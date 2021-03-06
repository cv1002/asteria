// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "executive_context.hpp"
#include "runtime_error.hpp"
#include "ptc_arguments.hpp"
#include "../llds/avmc_queue.hpp"
#include "../utilities.hpp"

namespace asteria {
namespace {

template<typename XRefT>
inline
Reference*
do_set_lazy_reference(Executive_Context& ctx, const phsh_string& name, XRefT&& xref)
  {
    auto& ref = ctx.open_named_reference(name);
    ref = ::std::forward<XRefT>(xref);
    return ::std::addressof(ref);
  }

}  // namespace

Executive_Context::
~Executive_Context()
  {
  }

void
Executive_Context::
do_bind_parameters(const rcptr<Variadic_Arguer>& zvarg, const cow_vector<phsh_string>& params,
                   Reference&& self, cow_vector<Reference>&& args)
  {
    // Set the zero-ary argument getter.
    this->m_zvarg = zvarg;

    // Set the `this` reference.
    // If the self reference is null, it is likely that `this` isn't ever referenced in this function,
    // so perform lazy initialization.
    if(!self.is_void() && !self.is_constant_null())
      this->open_named_reference(::rocket::sref("__this")) = ::std::move(self);

    // This is the subscript of the special parameter placeholder `...`.
    size_t elps = SIZE_MAX;

    // Set parameters, which are local references.
    for(size_t i = 0;  i < params.size();  ++i) {
      const auto& name = params.at(i);
      if(name.empty())
        continue;

      if(name.rdstr().starts_with("__"))
        ASTERIA_THROW("Reserved name not declarable as parameter (name `$1`)", name);

      if(name == "...") {
        // Nothing is set for the parameter placeholder, but the parameter list terminates here.
        ROCKET_ASSERT(i == params.size() - 1);
        elps = i;
        break;
      }
      // Set the parameter.
      if(ROCKET_UNEXPECT(i >= args.size()))
        this->open_named_reference(name) = Reference_root::S_constant();
      else
        this->open_named_reference(name) = ::std::move(args.mut(i));
    }

    // Disallow exceess arguments if the function is not variadic.
    if((elps == SIZE_MAX) && (args.size() > params.size())) {
      ASTERIA_THROW("Too many arguments (`$1` > `$2`)", args.size(), params.size());
    }
    args.erase(0, elps);

    // Stash variadic arguments for lazy initialization.
    // If all arguments are positional, `args` may be reused for the evaluation stack, so don't move it.
    if(args.size())
      this->m_lazy_args = ::std::move(args);
  }

void
Executive_Context::
do_defer_expression(const Source_Location& sloc, AVMC_Queue&& queue)
  {
    this->m_defer.emplace_back(sloc, ::std::move(queue));
  }

void
Executive_Context::
do_on_scope_exit_void()
  {
    // Execute all deferred expressions backwards.
    while(this->m_defer.size()) {
      // Pop an expression.
      auto pair = ::std::move(this->m_defer.mut_back());
      this->m_defer.pop_back();

      // Execute it. If an exception is thrown, append a frame and rethrow it.
      ASTERIA_RUNTIME_TRY {
        auto status = pair.second.execute(*this);
        ROCKET_ASSERT(status == air_status_next);
      }
      ASTERIA_RUNTIME_CATCH(Runtime_Error& except) {
        except.push_frame_defer(pair.first);
        this->do_on_scope_exit_exception(except);
        throw;
      }
    }
  }

void
Executive_Context::
do_on_scope_exit_return()
  {
    if(auto tca = this->m_stack->get_top().get_tail_call_opt()) {
      // If a PTC wrapper was returned, prepend all deferred expressions to it.
      auto& defer = tca->open_defer_stack();
      if(defer.size()) {
        defer.insert(defer.begin(), ::std::make_move_iterator(this->m_defer.mut_begin()),
                                    ::std::make_move_iterator(this->m_defer.mut_end()));
        this->m_defer.clear();
      }
      else
        defer.swap(this->m_defer);
    }
    else {
      // Stash the return reference.
      auto self = ::std::move(this->m_stack->open_top());

      // Evaluate all deferred expressions.
      this->do_on_scope_exit_void();

      // Restore the return reference.
      this->m_stack->clear();
      this->m_stack->push(::std::move(self));
    }
  }

void
Executive_Context::
do_on_scope_exit_exception(Runtime_Error& except)
  {
    // Execute all deferred expressions backwards.
    while(this->m_defer.size()) {
      // Pop an expression.
      auto pair = ::std::move(this->m_defer.mut_back());
      this->m_defer.pop_back();

      // Execute it. If an exception is thrown, replace `except` with it.
      ASTERIA_RUNTIME_TRY {
        auto status = pair.second.execute(*this);
        ROCKET_ASSERT(status == air_status_next);
      }
      ASTERIA_RUNTIME_CATCH(Runtime_Error& nested) {
        except = nested;
        except.push_frame_defer(pair.first);
      }
    }
  }

Reference*
Executive_Context::
do_lazy_lookup_opt(const phsh_string& name)
  {
    // Create pre-defined references as needed.
    // N.B. If you have ever changed these, remember to update 'analytic_context.cpp' as well.
    if(name == "__func") {
      // Note: This can only happen inside a function context.
      Reference_root::S_constant xref = { this->m_zvarg->func() };
      return do_set_lazy_reference(*this, name, ::std::move(xref));
    }

    if(name == "__this") {
      // Note: This can only happen inside a function context and the `this` argument is null.
      return do_set_lazy_reference(*this, name, Reference_root::S_constant());
    }

    if(name == "__varg") {
      // Note: This can only happen inside a function context.
      cow_function varg;
      if(ROCKET_EXPECT(this->m_lazy_args.size()))
        varg = ::rocket::make_refcnt<Variadic_Arguer>(*(this->m_zvarg), ::std::move(this->m_lazy_args));
      else
        varg = this->m_zvarg;

      Reference_root::S_constant xref = { ::std::move(varg) };
      return do_set_lazy_reference(*this, name, ::std::move(xref));
    }

    return nullptr;
  }

}  // namespace asteria
