// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "io.hpp"
#include "../runtime/argument_reader.hpp"
#include "../utilities.hpp"

namespace asteria {
namespace {

class IOF_Sentry
  {
  private:
    ::FILE* m_fp;

  public:
    explicit
    IOF_Sentry(::FILE* fp)
    noexcept
      : m_fp(fp)
      { ::flockfile(this->m_fp);  }

    ASTERIA_NONCOPYABLE_DESTRUCTOR(IOF_Sentry)
      { ::funlockfile(this->m_fp);  }

  public:
    operator
    ::FILE*()
    const noexcept
      { return this->m_fp;  }
  };

int
do_recover(const IOF_Sentry& fp)
  {
    // Note `errno` is meaningful only when an error has occurred. EOF is not an error.
    int err = 0;
    if(ROCKET_UNEXPECT(::ferror_unlocked(fp))) {
      // If the preceding operation failed for these non-fatal errors, clear the error bit.
      // This makes such operations retryable.
      // But note that we always throw an exception despite such recovery.
      err = errno;
      if(::rocket::is_any_of(err, { EINTR, EAGAIN, EWOULDBLOCK }))
        ::clearerr_unlocked(fp);
    }
    return err;
  }

size_t
do_write_utf8_common(const IOF_Sentry& fp, const cow_string& text)
  {
    size_t ncps = 0;
    size_t off = 0;
    while(off < text.size()) {
      // Decode a code point from `text`.
      char32_t cp;
      if(!utf8_decode(cp, text, off))
        ASTERIA_THROW("Invalid UTF-8 string (text `$1`, byte offset `$2`)", text, off);

      // Insert it into the output stream.
      if(::fputwc_unlocked(static_cast<wchar_t>(cp), fp) == WEOF)
        ASTERIA_THROW("Error writing standard output\n"
                      "[`fputwc_unlocked()` failed: $1]",
                      format_errno(errno));

      // The return value is the number of code points rather than bytes.
      ncps += 1;
    }
    return ncps;
  }

}  // namespace

optV_integer
std_io_getc()
  {
    // Lock standard input for reading.
    const IOF_Sentry fp(stdin);

    // Check stream status.
    if(::ferror_unlocked(fp))
      ASTERIA_THROW("Standard input failure (error bit set)");

    if(::fwide(fp, +1) < 0)
      ASTERIA_THROW("Invalid text read from binary-oriented input");

    // Read a UTF code point.
    wint_t wch = ::fgetwc_unlocked(fp);
    if(wch == WEOF) {
      // Throw an exception on error.
      // Return `null` on EOF.
      int err = do_recover(fp);
      if(err != 0)
        ASTERIA_THROW("Error reading standard input\n"
                      "[`fgetwc_unlocked()` failed: $1]",
                      format_errno(err));
      return nullopt;
    }

    // Zero-extend the code point to an integer.
    return static_cast<uint32_t>(wch);
  }

optV_string
std_io_getln()
  {
    // Lock standard input for reading.
    const IOF_Sentry fp(stdin);

    // Check stream status.
    if(::ferror_unlocked(fp))
      ASTERIA_THROW("Standard input failure (error bit set)");

    if(::fwide(fp, +1) < 0)
      ASTERIA_THROW("Invalid text read from binary-oriented input");

    // Read a UTF-8 string.
    cow_string u8str;
    for(;;) {
      wint_t wch = ::fgetwc_unlocked(fp);
      if(wch == WEOF) {
        // If at least a character has been read, don't fail.
        if(!u8str.empty())
          break;

        // Throw an exception on error.
        // Return `null` on EOF.
        int err = do_recover(fp);
        if(err != 0)
          ASTERIA_THROW("Error reading standard input\n"
                        "[`fgetwc_unlocked()` failed: $1]",
                        format_errno(err));
        return nullopt;
      }
      // If a LF is encountered, finish this line.
      if(wch == L'\n')
        break;

      // Append the non-LF character to the result string.
      char32_t cp = static_cast<uint32_t>(wch);
      if(!utf8_encode(u8str, cp))
        ASTERIA_THROW("Invalid UTF code point from standard input (value `$1`)", wch);
    }
    // Return the UTF-8 string.
    return u8str;
  }

optV_integer
std_io_putc(V_integer value)
  {
    // Lock standard output for writing.
    const IOF_Sentry fp(stdout);

    // Check stream status.
    if(::ferror_unlocked(fp))
      ASTERIA_THROW("Standard output failure (error bit set)");

    if(::fwide(fp, +1) < 0)
      ASTERIA_THROW("Invalid text write to binary-oriented output");

    // Validate the code point.
    char32_t cp = static_cast<uint32_t>(value);
    if(cp != value)
      ASTERIA_THROW("Invalid UTF code point (value `$1`)", value);

    // Check whether it is valid by try encoding it.
    // The result is discarded.
    char16_t sbuf[2];
    char16_t* sp = sbuf;
    if(!utf16_encode(sp, cp))
      ASTERIA_THROW("Invalid UTF code point (value `$1`)", value);

    // Write a UTF code point.
    if(::fputwc_unlocked(static_cast<wchar_t>(cp), fp) == WEOF)
      ASTERIA_THROW("Error writing standard output\n"
                    "[`fputwc_unlocked()` failed: $1]",
                    format_errno(errno));

    // Return the number of code points that have been written.
    // This is always 1 for this function.
    return 1;
  }

optV_integer
std_io_putc(V_string value)
  {
    // Lock standard output for writing.
    const IOF_Sentry fp(stdout);

    // Check stream status.
    if(::ferror_unlocked(fp))
      ASTERIA_THROW("Standard output failure (error bit set)");

    if(::fwide(fp, +1) < 0)
      ASTERIA_THROW("Invalid text write to binary-oriented output");

    // Write only the string.
    size_t ncps = do_write_utf8_common(fp, value);

    // Return the number of code points that have been written.
    return static_cast<int64_t>(ncps);
  }

optV_integer
std_io_putln(V_string value)
  {
    // Lock standard output for writing.
    const IOF_Sentry fp(stdout);

    // Check stream status.
    if(::ferror_unlocked(fp))
      ASTERIA_THROW("Standard output failure (error bit set)");

    if(::fwide(fp, +1) < 0)
      ASTERIA_THROW("Invalid text write to binary-oriented output");

    // Write the string itself.
    size_t ncps = do_write_utf8_common(fp, value);

    // Append a line feed and flush.
    if(::fputwc_unlocked(L'\n', fp) == WEOF)
      ASTERIA_THROW("Error writing standard output\n"
                    "[`fputwc_unlocked()` failed: $1]",
                    format_errno(errno));

    // Return the number of code points that have been written.
    // The implicit LF also counts.
    return static_cast<int64_t>(ncps + 1);
  }

optV_integer
std_io_putf(V_string templ, cow_vector<Value> values)
  {
    // Lock standard output for writing.
    const IOF_Sentry fp(stdout);

    // Check stream status.
    if(::ferror_unlocked(fp))
      ASTERIA_THROW("Standard output failure (error bit set)");

    if(::fwide(fp, +1) < 0)
      ASTERIA_THROW("Invalid text write to binary-oriented output");

    // Prepare inserters.
    cow_vector<::rocket::formatter> insts;
    insts.reserve(values.size());
    for(size_t i = 0;  i < values.size();  ++i)
      insts.push_back({
        [](tinyfmt& fmt, const void* ptr) -> tinyfmt&
          { return static_cast<const Value*>(ptr)->print(fmt);  },
        values.data() + i
      });

    // Compose the string into a stream.
    ::rocket::tinyfmt_str fmt;
    vformat(fmt, templ.data(), templ.size(), insts.data(), insts.size());

    // Write the string now.
    size_t ncps = do_write_utf8_common(fp, fmt.get_string());

    // Return the number of code points that have been written.
    return static_cast<int64_t>(ncps);
  }

optV_string
std_io_read(optV_integer limit)
  {
    size_t rlimit = (size_t)::rocket::clamp(limit.value_or(INT32_MAX), 0, 0x10'00000);

    // Lock standard input for reading.
    const IOF_Sentry fp(stdin);

    // Check stream status.
    if(::ferror_unlocked(fp))
      ASTERIA_THROW("Standard input failure (error bit set)");

    if(::fwide(fp, -1) > 0)
      ASTERIA_THROW("Invalid binary read from text-oriented input");

    // Read some bytes from the stream.
    cow_string data(rlimit, '\0');
    size_t ntotal = ::fread_unlocked(data.mut_data(), 1, data.size(), fp);
    if(ntotal == 0) {
      // Throw an exception on error.
      // Return `null` on EOF.
      int err = do_recover(fp);
      if(err != 0)
        ASTERIA_THROW("Error reading standard input\n"
                      "[`fread_unlocked()` failed: $1]",
                      format_errno(err));
      return nullopt;
    }

    // Return the byte string verbatim.
    return ::std::move(data.erase(ntotal));
  }

optV_integer
std_io_write(V_string data)
  {
    // Lock standard output for writing.
    const IOF_Sentry fp(stdout);

    // Check stream status.
    if(::ferror_unlocked(fp))
      ASTERIA_THROW("Standard output failure (error bit set)");

    if(::fwide(fp, -1) > 0)
      ASTERIA_THROW("Invalid binary write to text-oriented output");

    // Don't pass zero to `fwrite()`
    if(data.empty())
      return 0;

    // Write the byte string verbatim.
    size_t ntotal = ::fwrite_unlocked(data.data(), 1, data.size(), fp);
    if(ntotal == 0) {
      // Throw an exception on error.
      int err = do_recover(fp);
      if(err != 0)
        ASTERIA_THROW("Error writing standard output\n"
                      "[`fwrite_unlocked()` failed: $1]",
                      format_errno(errno));
    }

    // Return the number of bytes written.
    return static_cast<int64_t>(ntotal);
  }

void
std_io_flush()
  {
    // Flush standard output only.
    if(::fflush(stdout) == EOF)
      ASTERIA_THROW("Error flushing standard output\n"
                    "[`fflush()` failed: $1]",
                    format_errno(errno));
  }

void
create_bindings_io(V_object& result, API_Version /*version*/)
  {
    //===================================================================
    // `std.io.getc()`
    //===================================================================
    result.insert_or_assign(::rocket::sref("getc"),
      V_function(
"""""""""""""""""""""""""""""""""""""""""""""""" R"'''''''''''''''(
`std.io.getc()`

  * Reads a UTF code point from standard input.

  * Returns the code point that has been read as an integer. If the
    end of input is encountered, `null` is returned.

  * Throws an exception if standard input is binary-oriented, or if
    a read error occurs.
)'''''''''''''''" """""""""""""""""""""""""""""""""""""""""""""""",
*[](Reference& self, cow_vector<Reference>&& args, Global_Context& /*global*/) -> Reference&
  {
    Argument_Reader reader(::rocket::cref(args), ::rocket::sref("std.io.getc"));
    // Parse arguments.
    if(reader.I().F()) {
      Reference_root::S_temporary xref = { std_io_getc() };
      return self = ::std::move(xref);
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }
      ));

    //===================================================================
    // `std.io.getln()`
    //===================================================================
    result.insert_or_assign(::rocket::sref("getln"),
      V_function(
"""""""""""""""""""""""""""""""""""""""""""""""" R"'''''''''''''''(
`std.io.getln()`

  * Reads a UTF-8 string from standard input, which is terminated
    by either a LF character or the end of input. The terminating
    LF, if any, is not included in the returned string.

  * Returns the line that has been read as a string. If the end of
    input is encountered, `null` is returned.

  * Throws an exception if standard input is binary-oriented, or if
    a read error occurs, or if source data cannot be converted to a
    valid UTF code point sequence.
)'''''''''''''''" """""""""""""""""""""""""""""""""""""""""""""""",
*[](Reference& self, cow_vector<Reference>&& args, Global_Context& /*global*/) -> Reference&
  {
    Argument_Reader reader(::rocket::cref(args), ::rocket::sref("std.io.getln"));
    // Parse arguments.
    if(reader.I().F()) {
      Reference_root::S_temporary xref = { std_io_getln() };
      return self = ::std::move(xref);
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }
      ));

    //===================================================================
    // `std.io.putc()`
    //===================================================================
    result.insert_or_assign(::rocket::sref("putc"),
      V_function(
"""""""""""""""""""""""""""""""""""""""""""""""" R"'''''''''''''''(
`std.io.putc(value)`

  * Writes a UTF-8 string to standard output. `value` may be either
    an integer representing a UTF code point or a UTF-8 string.

  * Returns the number of UTF code points that have been written.

  * Throws an exception if standard output is binary-oriented, or
    if source data cannot be converted to a valid UTF code point
    sequence, or if a write error occurs.
)'''''''''''''''" """""""""""""""""""""""""""""""""""""""""""""""",
*[](Reference& self, cow_vector<Reference>&& args, Global_Context& /*global*/) -> Reference&
  {
    Argument_Reader reader(::rocket::cref(args), ::rocket::sref("std.io.putc"));
    // Parse arguments.
    V_integer ivalue;
    if(reader.I().v(ivalue).F()) {
      Reference_root::S_temporary xref = { std_io_putc(::std::move(ivalue)) };
      return self = ::std::move(xref);
    }
    V_string svalue;
    if(reader.I().v(svalue).F()) {
      Reference_root::S_temporary xref = { std_io_putc(::std::move(svalue)) };
      return self = ::std::move(xref);
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }
      ));

    //===================================================================
    // `std.io.putln()`
    //===================================================================
    result.insert_or_assign(::rocket::sref("putln"),
      V_function(
"""""""""""""""""""""""""""""""""""""""""""""""" R"'''''''''''''''(
`std.io.putln(text)`

  * Writes a UTF-8 string to standard output, followed by a LF,
    which may flush the stream automatically. `text` shall be a
    UTF-8 string.

  * Returns the number of UTF code points that have been written,
    including the terminating LF.

  * Throws an exception if standard output is binary-oriented, or
    if source data cannot be converted to a valid UTF code point
    sequence, or if a write error occurs.
)'''''''''''''''" """""""""""""""""""""""""""""""""""""""""""""""",
*[](Reference& self, cow_vector<Reference>&& args, Global_Context& /*global*/) -> Reference&
  {
    Argument_Reader reader(::rocket::cref(args), ::rocket::sref("std.io.putln"));
    // Parse arguments.
    V_string text;
    if(reader.I().v(text).F()) {
      Reference_root::S_temporary xref = { std_io_putln(::std::move(text)) };
      return self = ::std::move(xref);
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }
      ));

    //===================================================================
    // `std.io.putf()`
    //===================================================================
    result.insert_or_assign(::rocket::sref("putf"),
      V_function(
"""""""""""""""""""""""""""""""""""""""""""""""" R"'''''''''''''''(
`std.io.putf(templ, ...)`

 * Compose a string in the same way as `std.string.format()`, but
   instead of returning it, write it to standard output.

 * Returns the number of UTF code points that have been written.

 * Throws an exception if standard output is binary-oriented, or
   if source data cannot be converted to a valid UTF code point
   sequence, or if a write error occurs.
)'''''''''''''''" """""""""""""""""""""""""""""""""""""""""""""""",
*[](Reference& self, cow_vector<Reference>&& args, Global_Context& /*global*/) -> Reference&
  {
    Argument_Reader reader(::rocket::cref(args), ::rocket::sref("std.io.putf"));
    // Parse arguments.
    V_string templ;
    cow_vector<Value> values;
    if(reader.I().v(templ).F(values)) {
      Reference_root::S_temporary xref = { std_io_putf(::std::move(templ), ::std::move(values)) };
      return self = ::std::move(xref);
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }
      ));

    //===================================================================
    // `std.io.read()`
    //===================================================================
    result.insert_or_assign(::rocket::sref("read"),
      V_function(
"""""""""""""""""""""""""""""""""""""""""""""""" R"'''''''''''''''(
`std.io.read([limit])`

  * Reads a series of bytes from standard input. If `limit` is set,
    no more than this number of bytes will be read.

  * Returns the bytes that have been read as a string. If the end
    of input is encountered, `null` is returned.

  * Throws an exception if standard input is text-oriented, or if
    a read error occurs, or if source data cannot be converted to a
    valid UTF code point sequence.
)'''''''''''''''" """""""""""""""""""""""""""""""""""""""""""""""",
*[](Reference& self, cow_vector<Reference>&& args, Global_Context& /*global*/) -> Reference&
  {
    Argument_Reader reader(::rocket::cref(args), ::rocket::sref("std.io.read"));
    // Parse arguments.
    optV_integer limit;
    if(reader.I().o(limit).F()) {
      Reference_root::S_temporary xref = { std_io_read(::std::move(limit)) };
      return self = ::std::move(xref);
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }
      ));

    //===================================================================
    // `std.io.write()`
    //===================================================================
    result.insert_or_assign(::rocket::sref("write"),
      V_function(
"""""""""""""""""""""""""""""""""""""""""""""""" R"'''''''''''''''(
`std.io.write(data)`

  * Writes a series of bytes to standard output. `data` shall be a
    byte string.

  * Returns the number of bytes that have been written.

  * Throws an exception if standard output is text-oriented, or if
    a write error occurs.
)'''''''''''''''" """""""""""""""""""""""""""""""""""""""""""""""",
*[](Reference& self, cow_vector<Reference>&& args, Global_Context& /*global*/) -> Reference&
  {
    Argument_Reader reader(::rocket::cref(args), ::rocket::sref("std.io.write"));
    // Parse arguments.
    V_string data;
    if(reader.I().v(data).F()) {
      Reference_root::S_temporary xref = { std_io_write(::std::move(data)) };
      return self = ::std::move(xref);
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }
      ));

    //===================================================================
    // `std.io.flush()`
    //===================================================================
    result.insert_or_assign(::rocket::sref("flush"),
      V_function(
"""""""""""""""""""""""""""""""""""""""""""""""" R"'''''''''''''''(
`std.io.flush()`

  * Forces buffered data on standard output to be delivered to its
    underlying device. This function may be called regardless of
    the orientation of standard output.

  * Throws an exception if a write error occurs.
)'''''''''''''''" """""""""""""""""""""""""""""""""""""""""""""""",
*[](Reference& self, cow_vector<Reference>&& args, Global_Context& /*global*/) -> Reference&
  {
    Argument_Reader reader(::rocket::cref(args), ::rocket::sref("std.io.flush"));
    // Parse arguments.
    if(reader.I().F()) {
      std_io_flush();
      return self = Reference_root::S_void();
    }
    // Fail.
    reader.throw_no_matching_function_call();
  }
      ));
  }

}  // namespace asteria
