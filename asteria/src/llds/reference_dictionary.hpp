// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_LLDS_REFERENCE_DICTIONARY_HPP_
#define ASTERIA_LLDS_REFERENCE_DICTIONARY_HPP_

#include "../fwd.hpp"
#include "../runtime/reference.hpp"

namespace Asteria {

class Reference_Dictionary
  {
  private:
    struct Bucket
      {
        Bucket* next;  // the next bucket in the [non-circular] list
        Bucket* prev;  // the previous bucket in the [circular] list
        union { phsh_string kstor[1];  };  // initialized iff `prev` is non-null
        union { Reference vstor[1];  };  // initialized iff `prev` is non-null

        Bucket()
        { }

        ~Bucket()
        { }

        explicit operator
        bool()
        const
        noexcept
          { return this->prev != nullptr;  }
      };

    Bucket* m_bptr = nullptr;  // beginning of bucket storage
    Bucket* m_eptr = nullptr;  // end of bucket storage
    Bucket* m_head = nullptr;  // the first initialized bucket
    size_t m_size = 0;         // number of initialized buckets

  public:
    constexpr
    Reference_Dictionary()
    noexcept
      { }

    Reference_Dictionary(Reference_Dictionary&& other)
    noexcept
      { this->swap(other);  }

    Reference_Dictionary&
    operator=(Reference_Dictionary&& other)
    noexcept
      { return this->swap(other);  }

    ~Reference_Dictionary()
      {
        if(this->m_head)
          this->do_destroy_buckets();

        if(this->m_bptr)
          ::operator delete(this->m_bptr);
#ifdef ROCKET_DEBUG
        ::std::memset(static_cast<void*>(this), 0xA6, sizeof(*this));
#endif
      }

  private:
    void
    do_destroy_buckets()
    const
    noexcept;

    void
    do_enumerate_variables(Variable_Callback& callback)
    const;

    Bucket*
    do_xprobe(const phsh_string& name)
    const
    noexcept;

    void
    do_xrelocate_but(Bucket* qxcld)
    noexcept;

    inline
    void
    do_list_attach(Bucket* qbkt)
    noexcept;

    inline
    void
    do_list_detach(Bucket* qbkt)
    noexcept;

    void
    do_rehash(size_t nbkt);

    void
    do_attach(Bucket* qbkt, const phsh_string& name)
    noexcept;

    void
    do_detach(Bucket* qbkt)
    noexcept;

  public:
    bool
    empty()
    const
    noexcept
      { return this->m_head == nullptr;  }

    size_t
    size()
    const
    noexcept
      { return this->m_size;  }

    Reference_Dictionary&
    clear()
    noexcept
      {
        if(this->m_head)
          this->do_destroy_buckets();

        // Clean invalid data up.
        this->m_head = nullptr;
        this->m_size = 0;
        return *this;
      }

    Reference_Dictionary&
    swap(Reference_Dictionary& other)
    noexcept
      {
        xswap(this->m_bptr, other.m_bptr);
        xswap(this->m_eptr, other.m_eptr);
        xswap(this->m_head, other.m_head);
        xswap(this->m_size, other.m_size);
        return *this;
      }

    const Reference*
    get_opt(const phsh_string& name)
    const
    noexcept
      {
        // Be advised that `do_xprobe()` shall not be called when the table has not been allocated.
        if(!this->m_bptr) {
          return nullptr;
        }
        // Find the bucket for the name.
        auto qbkt = this->do_xprobe(name);
        if(!*qbkt) {
          // Not found.
          return nullptr;
        }
        ROCKET_ASSERT(qbkt->kstor[0].rdhash() == name.rdhash());
        return qbkt->vstor;
      }

    Reference&
    open(const phsh_string& name)
      {
        // Reserve more room by rehashing if the load factor would exceed 0.5.
        auto nbkt = static_cast<size_t>(this->m_eptr - this->m_bptr);
        if(ROCKET_UNEXPECT(this->m_size >= nbkt / 2)) {
          // Ensure the number of buckets is an odd number.
          this->do_rehash(this->m_size * 3 | 17);
        }
        // Find a bucket for the new name.
        auto qbkt = this->do_xprobe(name);
        if(*qbkt) {
          // Existent.
          return qbkt->vstor[0];
        }
        // Construct a null reference and return it.
        this->do_attach(qbkt, name);
        return qbkt->vstor[0];
      }

    bool
    erase(const phsh_string& name)
    noexcept
      {
        // Be advised that `do_xprobe()` shall not be called when the table has not been allocated.
        if(!this->m_bptr) {
          return false;
        }
        // Find the bucket for the name.
        auto qbkt = this->do_xprobe(name);
        if(!*qbkt) {
          // Not found.
          return false;
        }
        // Detach this reference.
        this->do_detach(qbkt);
        return true;
      }

    Variable_Callback&
    enumerate_variables(Variable_Callback& callback)
    const
      {
        this->do_enumerate_variables(callback);
        return callback;
      }
  };

inline
void
swap(Reference_Dictionary& lhs, Reference_Dictionary& rhs)
noexcept
  { lhs.swap(rhs);  }

}  // namespace Asteria

#endif
