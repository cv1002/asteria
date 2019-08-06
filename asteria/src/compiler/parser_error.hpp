// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_COMPILER_PARSER_ERROR_HPP_
#define ASTERIA_COMPILER_PARSER_ERROR_HPP_

#include "../fwd.hpp"

namespace Asteria {

class Parser_Error
  {
  public:
    enum Code : int64_t
      {
        // Special
        code_success                                    =    0,
        code_no_data_loaded                             = 1001,
        // Phase 1
        //   I/O stream
        //   UTF-8 decoder
        code_istream_open_failure                       = 2001,
        code_istream_badbit_set                         = 2002,
        code_utf8_sequence_invalid                      = 2003,
        code_utf8_sequence_incomplete                   = 2004,
        code_utf_code_point_invalid                     = 2005,
        code_null_character_disallowed                  = 2006,
        // Phase 2
        //   Tokenizer
        code_token_character_unrecognized               = 3001,
        code_string_literal_unclosed                    = 3002,
        code_escape_sequence_unknown                    = 3003,
        code_escape_sequence_incomplete                 = 3004,
        code_escape_sequence_invalid_hex                = 3005,
        code_escape_utf_code_point_invalid              = 3006,
        code_numeric_literal_incomplete                 = 3007,
        code_numeric_literal_suffix_disallowed          = 3008,
        code_numeric_literal_exponent_overflow          = 3009,
        code_integer_literal_overflow                   = 3010,
        code_integer_literal_exponent_negative          = 3011,
        code_real_literal_overflow                      = 3012,
        code_real_literal_underflow                     = 3013,
        code_block_comment_unclosed                     = 3014,
        code_digit_separator_following_nondigit         = 3015,
        // Phase 3
        //   Parser
        code_identifier_expected                        = 4002,
        code_semicolon_expected                         = 4003,
        code_string_literal_expected                    = 4004,
        code_statement_expected                         = 4005,
        code_equals_sign_expected                       = 4006,
        code_expression_expected                        = 4007,
        code_open_brace_expected                        = 4008,
        code_closed_brace_or_statement_expected         = 4009,
        code_open_parenthesis_expected                  = 4010,
        code_parameter_or_ellipsis_expected             = 4011,
        code_closed_parenthesis_expected                = 4012,
        code_colon_expected                             = 4013,
        code_closed_brace_or_switch_clause_expected     = 4014,
        code_keyword_while_expected                     = 4015,
        code_keyword_catch_expected                     = 4016,
        code_comma_expected                             = 4017,
        code_for_statement_initializer_expected         = 4018,
        code_semicolon_or_expression_expected           = 4019,
        code_closed_brace_expected                      = 4020,
        code_duplicate_object_key                       = 4021,
        code_closed_parenthesis_or_argument_expected    = 4022,
        code_closed_bracket_expected                    = 4023,
        code_open_brace_or_equal_initializer_expected   = 4024,
        code_equals_sign_or_colon_expected              = 4025,
      };

  public:
    ROCKET_PURE_FUNCTION static const char* describe_code(Code xcode) noexcept;

  private:
    int64_t m_line;
    size_t m_offset;
    size_t m_length;
    Code m_code;

  public:
    constexpr Parser_Error(int64_t xline, size_t xoffset, size_t xlength, Code xcode) noexcept
      : m_line(xline), m_offset(xoffset), m_length(xlength), m_code(xcode)
      {
      }

  public:
    constexpr int64_t line() const noexcept
      {
        return this->m_line;
      }
    constexpr size_t offset() const noexcept
      {
        return this->m_offset;
      }
    constexpr size_t length() const noexcept
      {
        return this->m_length;
      }
    constexpr Code code() const noexcept
      {
        return this->m_code;
      }

    std::ostream& print(std::ostream& ostrm) const;
    [[noreturn]] void convert_to_runtime_error_and_throw() const;
  };

constexpr bool operator==(const Parser_Error& lhs, Parser_Error::Code rhs) noexcept
  {
    return lhs.code() == rhs;
  }
constexpr bool operator!=(const Parser_Error& lhs, Parser_Error::Code rhs) noexcept
  {
    return lhs.code() != rhs;
  }

constexpr bool operator==(Parser_Error::Code lhs, const Parser_Error& rhs) noexcept
  {
    return lhs == rhs.code();
  }
constexpr bool operator!=(Parser_Error::Code lhs, const Parser_Error& rhs) noexcept
  {
    return lhs != rhs.code();
  }

inline std::ostream& operator<<(std::ostream& ostrm, const Parser_Error& error)
  {
    return error.print(ostrm);
  }

}  // namespace Asteria

#endif
