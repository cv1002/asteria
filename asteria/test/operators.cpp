// This file is part of Asteria.
// Copyleft 2018 - 2020, LH_Mouse. All wrongs reserved.

#include "utilities.hpp"
#include "../src/simple_script.hpp"
#include "../src/runtime/global_context.hpp"

using namespace asteria;

int main()
  {
    ::rocket::tinybuf_str cbuf;
    cbuf.set_string(::rocket::sref(
      R"__(
        var b = false, i = 12, r = 8.5, s = "a";
        var a = [ 1, 2, 3 ];
        var o = { one: 1, two: 2, three: 3 };

        assert ++i == 13;
        assert i == 13;
        assert ++r == 9.5;
        assert r == 9.5;

        assert --i == 12;
        assert i == 12;
        assert --r == 8.5;
        assert r == 8.5;

        assert a[0] == 1;
        assert a[1] == 2;
        assert a[2] == 3;
        assert a[3] == null;
        assert a[-1] == 3;
        assert a[-2] == 2;
        assert a[-3] == 1;
        assert a[-4] == null;
        assert o["one"] == 1;
        assert o["two"] == 2;
        assert o["three"] == 3;
        assert o["nonexistent"] == null;

        assert o.one == 1;
        assert o.two == 2;
        assert o.three == 3;
        assert o.nonexistent == null;

        assert +b == false;
        assert +i == 12;
        assert +r == 8.5;
        assert +s == 'a';

        assert -i == -12;
        assert -r == -8.5;

        assert ~b == true;
        assert ~i == -13;

        assert !b == true;
        assert !i == false;
        assert !r == false;
        assert !s == false;
        assert !a == false;
        assert !o == false;
        assert !0 == true;
        assert !0.0 == true;
        assert !"" == true;
        assert ![] == true;
        assert !{} == false;

        assert __sqrt 2 > 1.414;
        assert __sqrt 2 < 1.415;
        assert __sqrt +0.0 == 0;
        assert __sqrt -0.0 == 0;
        assert __sqrt infinity == infinity;

        assert __isinf 42 == false;
        assert __isinf 42.5 == false;
        assert __isinf +infinity == true;
        assert __isinf -infinity == true;
        assert __isinf nan == false;

        assert __isnan 42 == false;
        assert __isnan 42.5 == false;
        assert __isnan +infinity == false;
        assert __isnan -infinity == false;
        assert __isnan nan == true;

        assert __abs +12 == 12;
        assert __abs -12 == 12;
        assert __abs +1.5 == 1.5;
        assert __abs -1.5 == 1.5;
        assert __abs +infinity == infinity;
        assert __abs -infinity == infinity;
        assert __isnan __abs nan;

        assert __round +3 == +3;
        assert typeof __round +3 == "integer";
        assert __round -3 == -3;
        assert typeof __round -3 == "integer";
        assert __round +3.4 == +3;
        assert __round +3.5 == +4;
        assert typeof __round +3.5 == "real";
        assert __round -3.4 == -3;
        assert __round -3.5 == -4;
        assert typeof __round -3.5 == "real";

        assert __floor +3 == +3;
        assert typeof __floor +3 == "integer";
        assert __floor -3 == -3;
        assert typeof __floor -3 == "integer";
        assert __floor +3.4 == +3;
        assert __floor +3.5 == +3;
        assert typeof __floor +3.5 == "real";
        assert __floor -3.4 == -4;
        assert __floor -3.5 == -4;
        assert typeof __floor -3.5 == "real";

        assert __ceil +3 == +3;
        assert typeof __ceil +3 == "integer";
        assert __ceil -3 == -3;
        assert typeof __ceil -3 == "integer";
        assert __ceil +3.4 == +4;
        assert __ceil +3.5 == +4;
        assert typeof __ceil +3.5 == "real";
        assert __ceil -3.4 == -3;
        assert __ceil -3.5 == -3;
        assert typeof __ceil -3.5 == "real";

        assert __trunc +3 == +3;
        assert typeof __trunc +3 == "integer";
        assert __trunc -3 == -3;
        assert typeof __trunc -3 == "integer";
        assert __trunc +3.4 == +3;
        assert __trunc +3.5 == +3;
        assert typeof __trunc +3.5 == "real";
        assert __trunc -3.4 == -3;
        assert __trunc -3.5 == -3;
        assert typeof __trunc -3.5 == "real";

        assert __roundi +3 == +3;
        assert typeof __roundi +3 == "integer";
        assert __roundi -3 == -3;
        assert typeof __roundi -3 == "integer";
        assert __roundi +3.4 == +3;
        assert __roundi +3.5 == +4;
        assert typeof __roundi +3.5 == "integer";
        assert __roundi -3.4 == -3;
        assert __roundi -3.5 == -4;
        assert typeof __roundi -3.5 == "integer";

        assert __floori +3 == +3;
        assert typeof __floori +3 == "integer";
        assert __floori -3 == -3;
        assert typeof __floori -3 == "integer";
        assert __floori +3.4 == +3;
        assert __floori +3.5 == +3;
        assert typeof __floori +3.5 == "integer";
        assert __floori -3.4 == -4;
        assert __floori -3.5 == -4;
        assert typeof __floori -3.5 == "integer";

        assert __ceili +3 == +3;
        assert typeof __ceili +3 == "integer";
        assert __ceili -3 == -3;
        assert typeof __ceili -3 == "integer";
        assert __ceili +3.4 == +4;
        assert __ceili +3.5 == +4;
        assert typeof __ceili +3.5 == "integer";
        assert __ceili -3.4 == -3;
        assert __ceili -3.5 == -3;
        assert typeof __ceili -3.5 == "integer";

        assert __trunci +3 == +3;
        assert typeof __trunci +3 == "integer";
        assert __trunci -3 == -3;
        assert typeof __trunci -3 == "integer";
        assert __trunci +3.4 == +3;
        assert __trunci +3.5 == +3;
        assert typeof __trunci +3.5 == "integer";
        assert __trunci -3.4 == -3;
        assert __trunci -3.5 == -3;
        assert typeof __trunci -3.5 == "integer";

        assert i++ == 12;
        assert i == 13;
        assert r++ == 8.5;
        assert r == 9.5;

        assert i-- == 13;
        assert i == 12;
        assert r-- == 9.5;
        assert r == 8.5;

        assert unset a[1] == 2;
        assert a == [1, 3];
        assert unset a[10000] == null;
        assert unset o["one"] == 1;
        assert o.one == null;
        assert unset o.nonexistent == null;

        assert b * false == false;
        assert false * b == false;
        assert true * false == false;
        assert false * true == false;
        assert true * true == true;
        assert i * 3 == 36;
        assert 3 * i == 36;
        assert i * -3 == -36;
        assert -3 * i == -36;
        assert i * -1 == -12;
        assert -1 * i == -12;
        assert r * 3.0 == 25.5;
        assert 3.0 * r == 25.5;
        assert r * -3.0 == -25.5;
        assert -3.0 * r == -25.5;
        assert r * 5 == 42.5;
        assert 5 * r == 42.5;
        assert r * -5 == -42.5;
        assert -5 * r == -42.5;
        assert s * 3 == 'aaa';
        assert 3 * s == 'aaa';

        assert i / 5 == 2;
        assert 23 / i == 1;
        assert r / 2.0 == 4.25;
        assert 17.0 / r == 2.0;
        assert r / 17 == 0.5;
        assert 17 / 8.5 == 2.0;

        assert i % 7 == 5;
        assert 23 % i == 11;
        assert r % 1.125 == 0.625;
        assert 19.5 % r == 2.5;
        assert r % 2 == 0.5;
        assert 10 % r == 1.5;

        assert b + false == false;
        assert false + b == false;
        assert b + true == true;
        assert true + b == true;
        assert i + 2 == 14;
        assert 2 + i == 14;
        assert r + 2.0 == 10.5;
        assert 2.0 + r == 10.5;
        assert r + 3 == 11.5;
        assert 3 + r == 11.5;
        assert s + 'bc' == 'abc';
        assert 'bc' + s == 'bca';

        assert i - 3 == 9;
        assert 3 - i == -9;
        assert r - 3.25 == 5.25;
        assert 3.25 - r == -5.25;
        assert r - 3 == 5.5;
        assert 3 - r == -5.5;

        assert i <<< 3 == 96;
        assert -10 <<< 1 == -20;
        assert 'abc' <<< 1 == 'bc ';

        assert i >>> 3 == 1;
        assert -10 >>> 1 == 9223372036854775803;
        assert 'abc' >>> 1 == ' ab';

        assert i << 3 == 96;
        assert -10 << 1 == -20;
        assert 'abc' << 1 == 'abc ';

        assert i >> 3 == 1;
        assert -10 >> 1 == -5;
        assert 'abc' >> 1 == 'ab';

        assert false < true;
        assert 1 < 2;
        assert 1.0 < 2.0;
        assert 0x1.0p30 < infinity;
        assert "aa" < "b";

        assert true > false;
        assert 2 > 1;
        assert 2.0 > 1.0;
        assert -0x1.0p30 > -infinity;
        assert "aa" > "a";

        assert true >= true;
        assert false >= false;
        assert -1 >= -1;
        assert -1 >= -2;
        assert 10.0 >= 9.9;
        assert "bb" >= "bb";

        assert true <= true;
        assert false <= false;
        assert -1 <= -1;
        assert -1 <= 0;
        assert 10.0 <= 10.1;
        assert "bb" <= "bb";

        assert true == true;
        assert false == false;
        assert -2 == -2;
        assert "cd" == "cd";

        assert false != true;
        assert 1 != 0;
        assert nan != nan;
        assert "abc" != "def";
        assert false != null;
        assert "" != null;
        assert [] != 0;
        assert {} != [];

        assert (1 <=> 2) == -1;
        assert ("b" <=> "a") == 1;
        assert (true <=> true) == 0;
        assert ("false" <=> false) == "<unordered>";
        assert (1 <=> 1.0) == 0;
        assert (nan <=> 1.0) == "<unordered>";
        assert (nan <=> 1) == "<unordered>";

        assert (true & true) == true;
        assert (false & true) == false;
        assert (true & false) == false;
        assert (false & false) == false;
        assert (5 & 4) == 4;
        assert (-1 & -2) == -2;

        assert (true ^ true) == false;
        assert (false ^ true) == true;
        assert (true ^ false) == true;
        assert (false ^ false) == false;
        assert (5 ^ 4) == 1;
        assert (-1 ^ -2) == 1;

        assert (true | true) == true;
        assert (false | true) == true;
        assert (true | false) == true;
        assert (false | false) == false;
        assert (5 | 4) == 5;
        assert (-1 | -2) == -1;

        assert (1 && ++i) == 13;
        assert i == 13;
        assert (0 && ++i) == 0;
        assert i == 13;

        assert (1 || --i) == 1;
        assert i == 13;
        assert (0 || --i) == 12;
        assert i == 12;

        assert i ?? "abc" == 12;
        assert null ?? "abc" == "abc";
        assert null ?? null ?? 1 ?? null ?? 2 == 1;

        assert a == [1, 3];
        assert a[^] == 1;
        assert (a[^] = 'hello') == "hello";
        assert a == ['hello', 1, 3];
        assert unset a[^] == 'hello';
        assert a == [1, 3];
        assert unset a[^] == 1;
        assert a == [3];
        assert unset a[^] == 3;
        assert a == [];
        assert unset a[^] == null;

        assert a == [];
        assert (a[$] = 1) == 1;
        assert (a[$] = 3) == 3;
        assert a == [1, 3];
        assert a[$] == 3;
        assert (a[$] = 'meow') == "meow";
        assert a == [1, 3, 'meow'];
        assert unset a[$] == 'meow';
        assert a == [1, 3];
        assert unset a[$] == 3;
        assert a == [1];
        assert unset a[$] == 1;
        assert a == [];
        assert unset a[$] == null;
        assert a == [];

        assert __fma(+5, +6, +7) == +37;
        assert __fma(+5, -6, +7) == -23;
        assert      (0x1.0000000000003p-461 * 0x1.0000000000007p-461 + -0x1.000000000000Ap-922) ==           0;  // no fma
        assert __fma(0x1.0000000000003p-461 , 0x1.0000000000007p-461 , -0x1.000000000000Ap-922) == 0x1.5p-1022;  // fma
        assert      (0x1.0000000000001 * 1 + 0x0.00000000000007FF) == 0x1.0000000000001;  // no fma
        assert __fma(0x1.0000000000001 , 1 , 0x0.00000000000007FF) == 0x1.0000000000001;  // fma
        assert __fma(+5, -infinity, +7) == -infinity;
        assert __fma(+5, -6, +infinity) == +infinity;
        assert __isnan __fma(+infinity, +6, -infinity);
        assert __fma(+infinity, +6, +infinity) == +infinity;
        assert __isnan __fma(nan, 6, 7);
        assert __isnan __fma(5, nan, 7);
        assert __isnan __fma(5, 6, nan);

        assert ~"" == "";
        assert ~"\x01\x23\x45\x67\x89" == "\xFE\xDC\xBA\x98\x76";

        assert ("" & "") == "";
        assert ("987654321" & "abcdefghi") == "! #$%$# !";
        assert ("987654321" & "abcdefg") == "! #$%$#";
        assert ("987654" & "abcdefghi") == "! #$%$";

        assert ("" | "") == "";
        assert ("987654321" | "abcdefghi") == "yzwvuvwzy";
        assert ("987654321" | "abcdefg") == "yzwvuvw21";
        assert ("987654" | "abcdefghi") == "yzwvuvghi";

        assert ("" ^ "") == "";
        assert ("987654321" ^ "abcdefghi") == "XZTRPRTZX";
        assert ("987654321" ^ "abcdefg") == "XZTRPRT21";
        assert ("987654" ^ "abcdefghi") == "XZTRPRghi";

      )__"), tinybuf::open_read);
    Simple_Script code(cbuf, ::rocket::sref(__FILE__), 14);
    Global_Context global;
    code.execute(global);
  }
