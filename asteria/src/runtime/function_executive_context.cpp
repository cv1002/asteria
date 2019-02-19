// This file is part of Asteria.
// Copyleft 2018 - 2019, LH_Mouse. All wrongs reserved.

#include "../precompiled.hpp"
#include "function_executive_context.hpp"
#include "variadic_arguer.hpp"
#include "../utilities.hpp"

namespace Asteria {

Function_Executive_Context::~Function_Executive_Context()
  {
  }

    namespace {

    template<typename XcontainerT, typename XnameT, typename XvalueT> void do_predefine_constant(XcontainerT &predefs_out, XnameT &&xname, XvalueT &&xvalue)
      {
        Reference_Root::S_constant ref_c = { std::forward<XvalueT>(xvalue) };
        predefs_out.emplace_back(std::forward<XnameT>(xname), std::move(ref_c));
      }

    template<typename XcontainerT, typename XnameT, typename XrefT> void do_predefine_reference(XcontainerT &predefs_out, XnameT &&xname, XrefT &&xref)
      {
        predefs_out.emplace_back(std::forward<XnameT>(xname), std::forward<XrefT>(xref));
      }

    inline RefCnt_Object<Abstract_Function> do_make_varg(const RefCnt_Object<Variadic_Arguer> &zvarg, CoW_Vector<Reference> &&args)
      {
        if(ROCKET_EXPECT(args.empty())) {
          return zvarg;
        }
        return RefCnt_Object<Abstract_Function>(Variadic_Arguer(zvarg.get(), std::move(args)));
      }

    }

void Function_Executive_Context::do_set_arguments(const RefCnt_Object<Variadic_Arguer> &zvarg, const CoW_Vector<PreHashed_String> &params, Reference &&self, CoW_Vector<Reference> &&args)
  {
    // Set parameters, which are local variables.
    for(const auto &param : params) {
      if(param.empty()) {
        continue;
      }
      if(param.rdstr().starts_with("__")) {
        ASTERIA_THROW_RUNTIME_ERROR("The function parameter name `", param, "` is reserved and cannot be used.");
      }
      const auto index = static_cast<std::size_t>(&param - params.data());
      if(ROCKET_EXPECT(index >= args.size())) {
        // There is no argument for this parameter.
        this->open_named_reference(param) = Reference_Root::S_undefined();
        continue;
      }
      this->open_named_reference(param) = std::move(args.mut(index));
    }
    if(params.size() < args.size()) {
      args.erase(args.begin(), args.begin() + static_cast<std::ptrdiff_t>(params.size()));
    } else {
      args.clear();
    }
    // Set pre-defined variables.
    // N.B. You must keep these elements sorted.
    // N.B. If you have ever changed these, remember to update 'analytic_executive_context.cpp' as well.
    do_predefine_constant(this->m_predef_refs,
                          rocket::sref("__file"), D_string(zvarg->get_source_file()));
    do_predefine_constant(this->m_predef_refs,
                          rocket::sref("__func"), D_string(zvarg->get_function_signature()));
    do_predefine_constant(this->m_predef_refs,
                          rocket::sref("__line"), D_integer(zvarg->get_source_line()));
    do_predefine_reference(this->m_predef_refs,
                           rocket::sref("__this"), std::move(self));
    do_predefine_constant(this->m_predef_refs,
                          rocket::sref("__varg"), D_function(do_make_varg(zvarg, std::move(args))));
    // Set up them.
    this->do_set_named_reference_templates(this->m_predef_refs.data(), this->m_predef_refs.size());
  }

}
