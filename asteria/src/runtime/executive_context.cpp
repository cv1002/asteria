// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "executive_context.hpp"
#include "reference.hpp"
#include "variadic_arguer.hpp"
#include "../utilities.hpp"

namespace Asteria {

Executive_context::~Executive_context()
  {
  }

bool Executive_context::is_analytic() const noexcept
  {
    return false;
  }

const Executive_context * Executive_context::get_parent_opt() const noexcept
  {
    return this->m_parent_opt;
  }

    namespace {

    template<typename XnameT, typename XvalueT,
             ROCKET_ENABLE_IF(std::is_constructible<Value, XvalueT &&>::value)>
      void do_predefine(rocket::cow_vector<Reference_dictionary::Template> &predefs_out, XnameT &&xname, XvalueT &&xvalue)
      {
        Reference_root::S_constant ref_c = { std::forward<XvalueT>(xvalue) };
        predefs_out.emplace_back(std::forward<XnameT>(xname), std::move(ref_c));
      }
    template<typename XnameT, typename XrefT,
             ROCKET_ENABLE_IF(std::is_constructible<Reference, XrefT &&>::value)>
      void do_predefine(rocket::cow_vector<Reference_dictionary::Template> &predefs_out, XnameT &&xname, XrefT &&xref)
      {
        predefs_out.emplace_back(std::forward<XnameT>(xname), std::forward<XrefT>(xref));
      }

    }

void Executive_context::initialize_for_function(const rocket::refcounted_object<Variadic_arguer> &zvarg, const rocket::cow_vector<rocket::prehashed_string> &params, Reference &&self, rocket::cow_vector<Reference> &&args)
  {
    // Set parameters, which are local variables.
    for(std::size_t i = 0; i < params.size(); ++i) {
      const auto &param = params.at(i);
      if(param.empty()) {
        continue;
      }
      if(param.rdstr().starts_with("__")) {
        ASTERIA_THROW_RUNTIME_ERROR("The function parameter name `", param, "` is reserved and cannot be used.");
      }
      if(ROCKET_EXPECT(i < args.size())) {
        this->open_named_reference(param) = std::move(args.mut(i));
      } else {
        this->open_named_reference(param) /*= Reference_root::S_null()*/;
      }
    }
    // Set pre-defined variables.
    this->m_predef_refs.reserve(5);
    // N.B. You must keep these elements sorted.
    // N.B. If you have ever changed these, remember to update 'analytic_context.cpp' as well.
    do_predefine(this->m_predef_refs,
                 std::ref("__file"), D_string(zvarg.get().get_location().get_file()));
    do_predefine(this->m_predef_refs,
                 std::ref("__func"), D_string(zvarg.get().get_name()));
    do_predefine(this->m_predef_refs,
                 std::ref("__line"), D_integer(zvarg.get().get_location().get_line()));
    do_predefine(this->m_predef_refs,
                 std::ref("__this"), std::move(self));
    do_predefine(this->m_predef_refs,
                 std::ref("__varg"), D_function((params.size() < args.size())
                                                ? args.erase(args.begin(), args.begin() + static_cast<std::ptrdiff_t>(params.size())),
                                                  rocket::refcounted_object<Variadic_arguer>(zvarg.get(), std::move(args))
                                                : zvarg));
    // Set up them.
    this->do_set_named_reference_templates(this->m_predef_refs.data(), this->m_predef_refs.size());
  }

}
