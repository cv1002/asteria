// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#include "utilities.hpp"
#include "../src/simple_script.hpp"
#include "../src/runtime/global_context.hpp"

using namespace asteria;

int main()
  {
    Simple_Script code;
    code.reload_string(
      ::rocket::sref(__FILE__), __LINE__, ::rocket::sref(R"__(
///////////////////////////////////////////////////////////////////////////////

        var a = 1;
        func two() { return 2;  }
        func check() { return a &&= two();  }
        check();
        std.debug.logf("a = $1\n", a);

///////////////////////////////////////////////////////////////////////////////
      )__"));
    Global_Context global;
    code.execute(global);
  }
