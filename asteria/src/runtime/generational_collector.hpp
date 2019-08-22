// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#ifndef ASTERIA_RUNTIME_GENERATIONAL_COLLECTOR_HPP_
#define ASTERIA_RUNTIME_GENERATIONAL_COLLECTOR_HPP_

#include "../fwd.hpp"
#include "rcbase.hpp"
#include "collector.hpp"
#include "../llds/variable_hashset.hpp"

namespace Asteria {

class Generational_Collector final : public virtual Rcbase
  {
  private:
    // Mind the order of construction and destruction.
    Variable_HashSet m_pool;
    Collector m_oldest;
    Collector m_middle;
    Collector m_newest;

  public:
    Generational_Collector() noexcept
      : m_oldest(std::addressof(this->m_pool),                        nullptr,  10),
        m_middle(std::addressof(this->m_pool), std::addressof(this->m_oldest),  50),
        m_newest(std::addressof(this->m_pool), std::addressof(this->m_middle), 200)
      {
      }
    ~Generational_Collector() override;

    Generational_Collector(const Generational_Collector&)
      = delete;
    Generational_Collector& operator=(const Generational_Collector&)
      = delete;

  private:
    Collector Generational_Collector::* do_locate(GC_Generation gc_gen) const;

  public:
    size_t get_pool_size() const noexcept
      {
        return this->m_pool.size();
      }
    void clear_pool() noexcept
      {
        this->m_pool.clear();
      }

    const Collector& get_collector(GC_Generation gc_gen) const
      {
        return this->*(this->do_locate(gc_gen));
      }
    Collector& open_collector(GC_Generation gc_gen)
      {
        return this->*(this->do_locate(gc_gen));
      }

    rcptr<Variable> create_variable(GC_Generation gc_hint);
    size_t collect_variables(GC_Generation gc_limit);
  };

}  // namespace Asteria

#endif
