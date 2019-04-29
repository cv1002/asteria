// This file is part of Asteria.
// Copyleft 2018, LH_Mouse. All wrongs reserved.

#include "test_utilities.hpp"
#include "../src/compiler/simple_source_file.hpp"
#include "../src/runtime/global_context.hpp"
#include <sstream>

using namespace Asteria;

int main()
  {
    static constexpr char s_source[] =
      R"__(
        assert std.numeric.abs(+42) == 42;
        assert std.numeric.abs(-42) == 42;
        assert typeof std.numeric.abs(42) == "integer";
        assert std.numeric.abs(+42.5) == 42.5;
        assert std.numeric.abs(-42.5) == 42.5;
        assert std.numeric.abs(+infinity) == infinity;
        assert std.numeric.abs(-infinity) == infinity;
        assert std.numeric.abs(+nan) <=> 0 == "<unordered>";
        assert std.numeric.abs(-nan) <=> 0 == "<unordered>";
        assert typeof std.numeric.abs(42.5) == "real";

        assert std.numeric.sign(+42) ==  0;
        assert std.numeric.sign(-42) == -1;
        assert typeof std.numeric.sign(42) == "integer";
        assert std.numeric.sign(+42.5) ==  0;
        assert std.numeric.sign(-42.5) == -1;
        assert std.numeric.sign(+infinity) ==  0;
        assert std.numeric.sign(-infinity) == -1;
        assert std.numeric.sign(+nan) ==  0;
        assert std.numeric.sign(-nan) == -1;
        assert typeof std.numeric.sign(42.5) == "integer";

        assert std.numeric.is_finite(+5) == true;
        assert std.numeric.is_finite(-5) == true;
        assert std.numeric.is_finite(+3.5) == true;
        assert std.numeric.is_finite(-3.5) == true;
        assert std.numeric.is_finite(+3.5) == true;
        assert std.numeric.is_finite(-3.5) == true;
        assert std.numeric.is_finite(+infinity) == false;
        assert std.numeric.is_finite(-infinity) == false;
        assert std.numeric.is_finite(+nan) == false;
        assert std.numeric.is_finite(-nan) == false;

        assert std.numeric.is_infinity(+5) == false;
        assert std.numeric.is_infinity(-5) == false;
        assert std.numeric.is_infinity(+3.5) == false;
        assert std.numeric.is_infinity(-3.5) == false;
        assert std.numeric.is_infinity(+3.5) == false;
        assert std.numeric.is_infinity(-3.5) == false;
        assert std.numeric.is_infinity(+infinity) == true;
        assert std.numeric.is_infinity(-infinity) == true;
        assert std.numeric.is_infinity(+nan) == false;
        assert std.numeric.is_infinity(-nan) == false;
        assert std.numeric.is_infinity(+1.0 / 0);
        assert std.numeric.is_infinity(-1.0 / 0);

        assert std.numeric.is_nan(+5) == false;
        assert std.numeric.is_nan(-5) == false;
        assert std.numeric.is_nan(+3.5) == false;
        assert std.numeric.is_nan(-3.5) == false;
        assert std.numeric.is_nan(+3.5) == false;
        assert std.numeric.is_nan(-3.5) == false;
        assert std.numeric.is_nan(+infinity) == false;
        assert std.numeric.is_nan(-infinity) == false;
        assert std.numeric.is_nan(+nan) == true;
        assert std.numeric.is_nan(-nan) == true;
        assert std.numeric.is_nan(+0.0 / 0);
        assert std.numeric.is_nan(+0.0 / 0);

        assert std.numeric.clamp(1, 2, 3) == 2;
        assert std.numeric.clamp(2, 2, 3) == 2;
        assert std.numeric.clamp(3, 2, 3) == 3;
        assert std.numeric.clamp(4, 2, 3) == 3;
        assert typeof std.numeric.clamp(1, 2, 3) == "integer";
        assert std.numeric.clamp(1.5, 2, 3) == 2.0;
        assert std.numeric.clamp(2.5, 2, 3) == 2.5;
        assert std.numeric.clamp(3.5, 2, 3) == 3.0;
        assert typeof std.numeric.clamp(1.5, 2, 3) == "real";
        assert std.numeric.clamp(1, 2.5, 3.5) == 2.5;
        assert std.numeric.clamp(2, 2.5, 3.5) == 2.5;
        assert std.numeric.clamp(3, 2.5, 3.5) == 3.0;
        assert std.numeric.clamp(4, 2.5, 3.5) == 3.5;
        assert typeof std.numeric.clamp(1, 2.5, 3.5) == "real";
        assert std.numeric.clamp(1.75, 2.5, 3.5) == 2.50;
        assert std.numeric.clamp(2.75, 2.5, 3.5) == 2.75;
        assert std.numeric.clamp(3.75, 2.5, 3.5) == 3.50;
        assert typeof std.numeric.clamp(1.75, 2.5, 3.5) == "real";
        assert __isnan std.numeric.clamp(+nan, 2.5, 3.5);
        assert __isnan std.numeric.clamp(-nan, 2.5, 3.5);
        assert std.numeric.clamp(+infinity, 2.5, 3.5) == 3.5;
        assert std.numeric.clamp(-infinity, 2.5, 3.5) == 2.5;
        assert std.numeric.clamp(1, 2, 2) == 2;
        assert std.numeric.clamp(3, 2, 2) == 2;
        assert std.numeric.clamp(1, 2.5, 2.5) == 2.5;
        assert std.numeric.clamp(3, 2.5, 2.5) == 2.5;

        assert std.numeric.round(42) == 42;
        assert typeof std.numeric.round(42) == "integer";
        assert std.numeric.round(42.4) == 42;
        assert typeof std.numeric.round(42.4) == "real";
        assert std.numeric.round(42.5) == 43;
        assert typeof std.numeric.round(42.5) == "real";
        assert std.numeric.round(-42.4) == -42;
        assert typeof std.numeric.round(-42.4) == "real";
        assert std.numeric.round(-42.5) == -43;
        assert typeof std.numeric.round(-42.5) == "real";

        assert std.numeric.floor(42) == 42;
        assert typeof std.numeric.floor(42) == "integer";
        assert std.numeric.floor(42.4) == 42;
        assert typeof std.numeric.floor(42.4) == "real";
        assert std.numeric.floor(42.5) == 42;
        assert typeof std.numeric.floor(42.5) == "real";
        assert std.numeric.floor(-42.4) == -43;
        assert typeof std.numeric.floor(-42.4) == "real";
        assert std.numeric.floor(-42.5) == -43;
        assert typeof std.numeric.floor(-42.5) == "real";

        assert std.numeric.ceil(42) == 42;
        assert typeof std.numeric.ceil(42) == "integer";
        assert std.numeric.ceil(42.4) == 43;
        assert typeof std.numeric.ceil(42.4) == "real";
        assert std.numeric.ceil(42.5) == 43;
        assert typeof std.numeric.ceil(42.5) == "real";
        assert std.numeric.ceil(-42.4) == -42;
        assert typeof std.numeric.ceil(-42.4) == "real";
        assert std.numeric.ceil(-42.5) == -42;
        assert typeof std.numeric.ceil(-42.5) == "real";

        assert std.numeric.trunc(42) == 42;
        assert typeof std.numeric.trunc(42) == "integer";
        assert std.numeric.trunc(42.4) == 42;
        assert typeof std.numeric.trunc(42.4) == "real";
        assert std.numeric.trunc(42.5) == 42;
        assert typeof std.numeric.trunc(42.5) == "real";
        assert std.numeric.trunc(-42.4) == -42;
        assert typeof std.numeric.trunc(-42.4) == "real";
        assert std.numeric.trunc(-42.5) == -42;
        assert typeof std.numeric.trunc(-42.5) == "real";

        assert std.numeric.iround(42) == 42;
        assert typeof std.numeric.iround(42) == "integer";
        assert std.numeric.iround(42.4) == 42;
        assert typeof std.numeric.iround(42.4) == "integer";
        assert std.numeric.iround(42.5) == 43;
        assert typeof std.numeric.iround(42.5) == "integer";
        assert std.numeric.iround(-42.4) == -42;
        assert typeof std.numeric.iround(-42.4) == "integer";
        assert std.numeric.iround(-42.5) == -43;
        assert typeof std.numeric.iround(-42.5) == "integer";

        assert std.numeric.ifloor(42) == 42;
        assert typeof std.numeric.ifloor(42) == "integer";
        assert std.numeric.ifloor(42.4) == 42;
        assert typeof std.numeric.ifloor(42.4) == "integer";
        assert std.numeric.ifloor(42.5) == 42;
        assert typeof std.numeric.ifloor(42.5) == "integer";
        assert std.numeric.ifloor(-42.4) == -43;
        assert typeof std.numeric.ifloor(-42.4) == "integer";
        assert std.numeric.ifloor(-42.5) == -43;
        assert typeof std.numeric.ifloor(-42.5) == "integer";

        assert std.numeric.iceil(42) == 42;
        assert typeof std.numeric.iceil(42) == "integer";
        assert std.numeric.iceil(42.4) == 43;
        assert typeof std.numeric.iceil(42.4) == "integer";
        assert std.numeric.iceil(42.5) == 43;
        assert typeof std.numeric.iceil(42.5) == "integer";
        assert std.numeric.iceil(-42.4) == -42;
        assert typeof std.numeric.iceil(-42.4) == "integer";
        assert std.numeric.iceil(-42.5) == -42;
        assert typeof std.numeric.iceil(-42.5) == "integer";

        assert std.numeric.itrunc(42) == 42;
        assert typeof std.numeric.itrunc(42) == "integer";
        assert std.numeric.itrunc(42.4) == 42;
        assert typeof std.numeric.itrunc(42.4) == "integer";
        assert std.numeric.itrunc(42.5) == 42;
        assert typeof std.numeric.itrunc(42.5) == "integer";
        assert std.numeric.itrunc(-42.4) == -42;
        assert typeof std.numeric.itrunc(-42.4) == "integer";
        assert std.numeric.itrunc(-42.5) == -42;
        assert typeof std.numeric.itrunc(-42.5) == "integer";

        assert std.numeric.random() >= 0.0;
        assert std.numeric.random() <  1.0;
        assert typeof std.numeric.random() == "real";
        assert std.numeric.random(2) >= 0;
        assert std.numeric.random(2) <  2;
        assert typeof std.numeric.random(2) == "integer";
        assert std.numeric.random(1.5) >= 0.0;
        assert std.numeric.random(1.5) <  1.5;
        assert typeof std.numeric.random(1.5) == "real";
        assert std.numeric.random(-1, 1) >= -1;
        assert std.numeric.random(-1, 1) <   1;
        assert typeof std.numeric.random(-1, 1) == "integer";
        assert std.numeric.random(-1.5, -0.5) >= -1.5;
        assert std.numeric.random(-1.5, -0.5) <  -0.5;
        assert typeof std.numeric.random(-1.5, -0.5) == "real";

        assert std.numeric.sqrt(+16) == 4;
        assert __isnan std.numeric.sqrt(-16);
        assert std.numeric.sqrt(+0.0) == 0;
        assert std.numeric.sqrt(-0.0) == 0;
        assert std.numeric.sqrt(+infinity) == infinity;
        assert __isnan std.numeric.sqrt(-infinity);
        assert __isnan std.numeric.sqrt(nan);

        assert std.numeric.fma(+5, +6, +7) == +37;
        assert std.numeric.fma(+5, -6, +7) == -23;
        assert                (0x1.0000000000003p-461 * 0x1.0000000000007p-461 + -0x1.000000000000Ap-922) ==           0;  // no fma
        assert std.numeric.fma(0x1.0000000000003p-461,  0x1.0000000000007p-461,  -0x1.000000000000Ap-922) == 0x1.5p-1022;  // fma
        assert std.numeric.fma(+5, -infinity, +7) == -infinity;
        assert std.numeric.fma(+5, -6, +infinity) == +infinity;
        assert __isnan std.numeric.fma(+infinity, +6, -infinity);
        assert std.numeric.fma(+infinity, +6, +infinity) == +infinity;
        assert __isnan std.numeric.fma(nan, 6, 7);
        assert __isnan std.numeric.fma(5, nan, 7);
        assert __isnan std.numeric.fma(5, 6, nan);

        assert std.numeric.addm(+1, +2) == +3;
        assert std.numeric.addm(+1, -2) == -1;
        assert std.numeric.addm(std.constants.integer_max, +2) == std.constants.integer_min + 1;
        assert std.numeric.addm(std.constants.integer_min, -2) == std.constants.integer_max - 1;
        assert std.numeric.addm(+2, std.constants.integer_max) == std.constants.integer_min + 1;
        assert std.numeric.addm(-2, std.constants.integer_min) == std.constants.integer_max - 1;

        assert std.numeric.subm(+1, +2) == -1;
        assert std.numeric.subm(+1, -2) == +3;
        assert std.numeric.subm(std.constants.integer_max, -2) == std.constants.integer_min + 1;
        assert std.numeric.subm(std.constants.integer_min, +2) == std.constants.integer_max - 1;
        assert std.numeric.subm(-2, std.constants.integer_max) == std.constants.integer_max;
        assert std.numeric.subm(+2, std.constants.integer_min) == std.constants.integer_min + 2;

        assert std.numeric.mulm(+2, +3) == +6;
        assert std.numeric.mulm(+2, -3) == -6;
        assert std.numeric.mulm(-2, +3) == -6;
        assert std.numeric.mulm(-2, -3) == +6;
        assert std.numeric.mulm(std.constants.integer_max, +3) == std.constants.integer_max - 2;
        assert std.numeric.mulm(std.constants.integer_max, -3) == std.constants.integer_min + 3;
        assert std.numeric.mulm(+3, std.constants.integer_max) == std.constants.integer_max - 2;
        assert std.numeric.mulm(-3, std.constants.integer_max) == std.constants.integer_min + 3;
        assert std.numeric.mulm(std.constants.integer_min, +3) == std.constants.integer_min;
        assert std.numeric.mulm(std.constants.integer_min, -3) == std.constants.integer_min;
        assert std.numeric.mulm(+3, std.constants.integer_min) == std.constants.integer_min;
        assert std.numeric.mulm(-3, std.constants.integer_min) == std.constants.integer_min;

        assert std.numeric.adds(+2, +3) == +5;
        assert std.numeric.adds(+2, -3) == -1;
        assert std.numeric.adds(std.constants.integer_max, +3) == std.constants.integer_max;
        assert std.numeric.adds(std.constants.integer_min, -3) == std.constants.integer_min;
        assert std.numeric.adds(+3, std.constants.integer_max) == std.constants.integer_max;
        assert std.numeric.adds(-3, std.constants.integer_min) == std.constants.integer_min;
        assert std.numeric.adds(std.constants.integer_max, std.constants.integer_max) == std.constants.integer_max;
        assert std.numeric.adds(std.constants.integer_min, std.constants.integer_min) == std.constants.integer_min;
        assert std.numeric.adds(+2, +3, +4, +8) == +5;
        assert std.numeric.adds(+2, -3, +4, +8) == +4;
        assert std.numeric.adds(+2, +9, +4, +8) == +8;
        assert std.numeric.adds(+2.5, +3.75) == +6.25;
        assert std.numeric.adds(+2.5, -3.75) == -1.25;
        assert std.numeric.adds(+infinity, +infinity) == +infinity;
        assert __isnan std.numeric.adds(+infinity, -infinity);
        assert __isnan std.numeric.adds(-infinity, +infinity);
        assert std.numeric.adds(-infinity, -infinity) == -infinity;
        assert __isnan std.numeric.adds(nan, 42);
        assert __isnan std.numeric.adds(42, nan);
        assert std.numeric.adds(+2.5, +3.75, -1, +7.5) == +6.25;
        assert std.numeric.adds(+2.5, -3.75, -1, +7.5) == -1.00;
        assert std.numeric.adds(+2.5, +9.75, -1, +7.5) == +7.50;
        assert std.numeric.adds(+infinity, +infinity, 3, 9) == 9;
        assert __isnan std.numeric.adds(+infinity, -infinity, 3, 9);
        assert __isnan std.numeric.adds(-infinity, +infinity, 3, 9);
        assert std.numeric.adds(-infinity, -infinity, 3, 9) == 3;
        assert __isnan std.numeric.adds(nan, 42, 3, 9);
        assert __isnan std.numeric.adds(42, nan, 3, 9);

        assert std.numeric.subs(+2, +3) == -1;
        assert std.numeric.subs(+2, -3) == +5;
        assert std.numeric.subs(std.constants.integer_max, -3) == std.constants.integer_max;
        assert std.numeric.subs(std.constants.integer_min, +3) == std.constants.integer_min;
        assert std.numeric.subs(-3, std.constants.integer_max) == std.constants.integer_min;
        assert std.numeric.subs(+3, std.constants.integer_min) == std.constants.integer_max;
        assert std.numeric.subs(std.constants.integer_max, std.constants.integer_min) == std.constants.integer_max;
        assert std.numeric.subs(std.constants.integer_min, std.constants.integer_max) == std.constants.integer_min;
        assert std.numeric.subs(+2, +3, +4, +8) == +4;
        assert std.numeric.subs(+2, -3, +4, +8) == +5;
        assert std.numeric.subs(+9, -2, +4, +8) == +8;
        assert std.numeric.subs(+2.5, +3.75) == -1.25;
        assert std.numeric.subs(+2.5, -3.75) == +6.25;
        assert __isnan std.numeric.subs(+infinity, +infinity);
        assert std.numeric.subs(+infinity, -infinity) == +infinity;
        assert std.numeric.subs(-infinity, +infinity) == -infinity;
        assert __isnan std.numeric.subs(-infinity, -infinity);
        assert __isnan std.numeric.subs(nan, 42);
        assert __isnan std.numeric.subs(42, nan);
        assert std.numeric.subs(+2.5, +3.75, -1, +7.5) == -1.00;
        assert std.numeric.subs(+2.5, -3.75, -1, +7.5) == +6.25;
        assert std.numeric.subs(+7.0, -2.25, -1, +7.5) == +7.50;
        assert __isnan std.numeric.subs(+infinity, +infinity, 3, 9);
        assert std.numeric.subs(+infinity, -infinity, 3, 9) == 9;
        assert std.numeric.subs(-infinity, +infinity, 3, 9) == 3;
        assert __isnan std.numeric.subs(-infinity, -infinity, 3, 9);
        assert __isnan std.numeric.subs(nan, 42, 3, 9);
        assert __isnan std.numeric.subs(42, nan, 3, 9);

        assert std.numeric.muls(+2, +3) == +6;
        assert std.numeric.muls(+2, -3) == -6;
        assert std.numeric.muls(std.constants.integer_max, +1) == std.constants.integer_max;
        assert std.numeric.muls(std.constants.integer_max, -1) == std.constants.integer_min + 1;
        assert std.numeric.muls(std.constants.integer_min, +1) == std.constants.integer_min;
        assert std.numeric.muls(std.constants.integer_min, -1) == std.constants.integer_max;
        assert std.numeric.muls(+1, std.constants.integer_max) == std.constants.integer_max;
        assert std.numeric.muls(-1, std.constants.integer_max) == std.constants.integer_min + 1;
        assert std.numeric.muls(+1, std.constants.integer_min) == std.constants.integer_min;
        assert std.numeric.muls(-1, std.constants.integer_min) == std.constants.integer_max;
        assert std.numeric.muls(std.constants.integer_max, +3) == std.constants.integer_max;
        assert std.numeric.muls(std.constants.integer_max, -3) == std.constants.integer_min;
        assert std.numeric.muls(std.constants.integer_min, +3) == std.constants.integer_min;
        assert std.numeric.muls(std.constants.integer_min, -3) == std.constants.integer_max;
        assert std.numeric.muls(+3, std.constants.integer_max) == std.constants.integer_max;
        assert std.numeric.muls(-3, std.constants.integer_max) == std.constants.integer_min;
        assert std.numeric.muls(+3, std.constants.integer_min) == std.constants.integer_min;
        assert std.numeric.muls(-3, std.constants.integer_min) == std.constants.integer_max;
        assert std.numeric.muls(std.constants.integer_max, std.constants.integer_max) == std.constants.integer_max;
        assert std.numeric.muls(std.constants.integer_max, std.constants.integer_min) == std.constants.integer_min;
        assert std.numeric.muls(std.constants.integer_min, std.constants.integer_max) == std.constants.integer_min;
        assert std.numeric.muls(std.constants.integer_min, std.constants.integer_min) == std.constants.integer_max;
        assert std.numeric.muls(+2, +3, +4, +8) == +6;
        assert std.numeric.muls(+2, -3, +4, +8) == +4;
        assert std.numeric.muls(+2, +9, +4, +8) == +8;
        assert std.numeric.muls(+2.5, +3.75) == +9.375;
        assert std.numeric.muls(+2.5, -3.75) == -9.375;
        assert std.numeric.muls(+infinity, +infinity) == +infinity;
        assert std.numeric.muls(+infinity, -infinity) == -infinity;
        assert std.numeric.muls(-infinity, +infinity) == -infinity;
        assert std.numeric.muls(-infinity, -infinity) == +infinity;
        assert __isnan std.numeric.muls(nan, 42);
        assert __isnan std.numeric.muls(42, nan);
        assert std.numeric.muls(+2.5, +3.75, -1, +9.5) == +9.375;
        assert std.numeric.muls(+2.5, -3.75, -1, +9.5) == -1.000;
        assert std.numeric.muls(+2.5, +9.75, -1, +9.5) == +9.500;
        assert std.numeric.muls(+infinity, +infinity, 3, 9) == 9;
        assert std.numeric.muls(+infinity, -infinity, 3, 9) == 3;
        assert std.numeric.muls(-infinity, +infinity, 3, 9) == 3;
        assert std.numeric.muls(-infinity, -infinity, 3, 9) == 9;
        assert __isnan std.numeric.muls(nan, 42, 3, 9);
        assert __isnan std.numeric.muls(42, nan, 3, 9);

        assert std.numeric.format(+1234) == "1234";
        assert std.numeric.format(+0) == "0";
        assert std.numeric.format(-0) == "0";
        assert std.numeric.format(-1234) == "-1234";
        assert std.numeric.format(+9223372036854775807) == "9223372036854775807";
        assert std.numeric.format(-9223372036854775808) == "-9223372036854775808";
        assert std.numeric.format(+0x1234,  2) == "0b1001000110100";
        assert std.numeric.format(-0x1234,  2) == "-0b1001000110100";
        assert std.numeric.format(+0x1234, 10) == "4660";
        assert std.numeric.format(-0x1234, 10) == "-4660";
        assert std.numeric.format(+0x1234, 16) == "0x1234";
        assert std.numeric.format(-0x1234, 16) == "-0x1234";
        assert std.numeric.format(+0x1234,  2,  2) == "0b10010001101p2";
        assert std.numeric.format(-0x1234,  2,  2) == "-0b10010001101p2";
        assert std.numeric.format(+0x1234, 10,  2) == "1165p2";
        assert std.numeric.format(-0x1234, 10,  2) == "-1165p2";
        assert std.numeric.format(+0x1234, 16,  2) == "0x48Dp2";
        assert std.numeric.format(-0x1234, 16,  2) == "-0x48Dp2";
        assert std.numeric.format(+0x1234,  2, 10) == "0b111010010e1";
        assert std.numeric.format(-0x1234,  2, 10) == "-0b111010010e1";
        assert std.numeric.format(+0x1234, 10, 10) == "466e1";
        assert std.numeric.format(-0x1234, 10, 10) == "-466e1";
        assert std.numeric.format(+123.75) == "123.75";
        assert std.numeric.format(+0.0) == "0";
        assert std.numeric.format(-0.0) == "-0";
        assert std.numeric.format(-123.75) == "-123.75";
        assert std.numeric.format(+infinity) == "infinity";
        assert std.numeric.format(-infinity) == "-infinity";
        assert std.numeric.format(+nan) == "nan";
        assert std.numeric.format(-nan) == "-nan";
        assert std.numeric.format(+0x123.C,  2) == "0b100100011.11";
        assert std.numeric.format(-0x123.C,  2) == "-0b100100011.11";
        assert std.numeric.format(+0x123.C, 10) == "291.75";
        assert std.numeric.format(-0x123.C, 10) == "-291.75";
        assert std.numeric.format(+0x123.C, 16) == "0x123.C";
        assert std.numeric.format(-0x123.C, 16) == "-0x123.C";
        assert std.numeric.format(+0x0.0123C,  2) == "0b0.000000010010001111";
        assert std.numeric.format(-0x0.0123C,  2) == "-0b0.000000010010001111";
        assert std.numeric.format(+0x0.0123C, 10) == "0.004451751708984375";
        assert std.numeric.format(-0x0.0123C, 10) == "-0.004451751708984375";
        assert std.numeric.format(+0x0.0123C, 16) == "0x0.0123C";
        assert std.numeric.format(-0x0.0123C, 16) == "-0x0.0123C";
        assert std.numeric.format(+0x123.C,  2,  2) == "0b1.0010001111p8";
        assert std.numeric.format(-0x123.C,  2,  2) == "-0b1.0010001111p8";
        assert std.numeric.format(+0x123.C, 10,  2) == "1.1396484375p8";
        assert std.numeric.format(-0x123.C, 10,  2) == "-1.1396484375p8";
        assert std.numeric.format(+0x123.C, 16,  2) == "0x1.23Cp8";
        assert std.numeric.format(-0x123.C, 16,  2) == "-0x1.23Cp8";
        assert std.numeric.format(+0x0.0123C,  2,  2) == "0b1.0010001111p-8";
        assert std.numeric.format(-0x0.0123C,  2,  2) == "-0b1.0010001111p-8";
        assert std.numeric.format(+0x0.0123C, 10,  2) == "1.1396484375p-8";
        assert std.numeric.format(-0x0.0123C, 10,  2) == "-1.1396484375p-8";
        assert std.numeric.format(+0x0.0123C, 16,  2) == "0x1.23Cp-8";
        assert std.numeric.format(-0x0.0123C, 16,  2) == "-0x1.23Cp-8";

//        assert std.numeric.parse_integer(" 1234") == +1234;
//        assert std.numeric.parse_integer("+1234") == +1234;
//        assert std.numeric.parse_integer("-1234") == -1234;
//        assert std.numeric.parse_integer(" 0   ") == 0;
//        assert std.numeric.parse_integer("+0   ") == 0;
//        assert std.numeric.parse_integer("-0   ") == 0;
//        assert std.numeric.parse_integer(" 0b0100001100100001  ") == +0x4321;
//        assert std.numeric.parse_integer("+0b0100001100100001  ") == +0x4321;
//        assert std.numeric.parse_integer("-0b0100001100100001  ") == -0x4321;
//        assert std.numeric.parse_integer(" 0b0100001100100001p3") == +0x21908;
//        assert std.numeric.parse_integer("+0b0100001100100001p3") == +0x21908;
//        assert std.numeric.parse_integer("-0b0100001100100001p3") == -0x21908;
//        assert std.numeric.parse_integer(" 0b0100001100100001e3") == +0x10638e8;
//        assert std.numeric.parse_integer("+0b0100001100100001e3") == +0x10638e8;
//        assert std.numeric.parse_integer("-0b0100001100100001e3") == -0x10638e8;
//        assert std.numeric.parse_integer(" 0x4321  ") == +0x4321;
//        assert std.numeric.parse_integer("+0x4321  ") == +0x4321;
//        assert std.numeric.parse_integer("-0x4321  ") == -0x4321;
//        assert std.numeric.parse_integer(" 0x4321p3") == +0x21908;
//        assert std.numeric.parse_integer("+0x4321p3") == +0x21908;
//        assert std.numeric.parse_integer("-0x4321p3") == -0x21908;
//        assert std.numeric.parse_integer(" 0x4321e3") == +0x10638e8;
//        assert std.numeric.parse_integer("+0x4321e3") == +0x10638e8;
//        assert std.numeric.parse_integer("-0x4321e3") == -0x10638e8;
//        assert std.numeric.parse_integer(" 999999999999999999999999999999") == null;
//        assert std.numeric.parse_integer("+999999999999999999999999999999") == null;
//        assert std.numeric.parse_integer("-999999999999999999999999999999") == null;
//        assert std.numeric.parse_integer(" 1e1000000") == null;
//        assert std.numeric.parse_integer("+1e1000000") == null;
//        assert std.numeric.parse_integer("-1e1000000") == null;
//        assert std.numeric.parse_integer(" 0e1000000") == 0;
//        assert std.numeric.parse_integer("+0e1000000") == 0;
//        assert std.numeric.parse_integer("-0e1000000") == 0;
//
//        assert std.numeric.parse_real(" 1234") == +1234;
//        assert std.numeric.parse_real("+1234") == +1234;
//        assert std.numeric.parse_real("-1234") == -1234;
//        assert std.numeric.parse_real(" 123.75") == +123.75;
//        assert std.numeric.parse_real("+123.75") == +123.75;
//        assert std.numeric.parse_real("-123.75") == -123.75;
//        assert std.numeric.parse_real(" 0   ") == 0;
//        assert std.numeric.parse_real("+0   ") == 0;
//        assert __signbit std.numeric.parse_real("+0   ") ==  0;
//        assert std.numeric.parse_real("-0   ") == 0;
//        assert __signbit std.numeric.parse_real("-0   ") == -1;
//        assert std.numeric.parse_real(" infinity") == +infinity;
//        assert std.numeric.parse_real("+infinity") == +infinity;
//        assert std.numeric.parse_real("-infinity") == -infinity;
//        assert __isnan std.numeric.parse_real(" nan");
//        assert __isnan std.numeric.parse_real("+nan");
//        assert __isnan std.numeric.parse_real("-nan");
//        assert std.numeric.parse_real(" 0b01000011.00100001  ") == +67.1289;
//        assert std.numeric.parse_real("+0b01000011.00100001  ") == +67.1289;
//        assert std.numeric.parse_real("-0b01000011.00100001  ") == -67.1289;
//        assert std.numeric.parse_real(" 0b01000011.00100001p3") == +67.1289;
//        assert std.numeric.parse_real("+0b01000011.00100001p3") == +67.1289;
//        assert std.numeric.parse_real("-0b01000011.00100001p3") == -67.1289;
//        assert std.numeric.parse_real(" 0b01000011.00100001e3") == +67128.9;
//        assert std.numeric.parse_real("+0b01000011.00100001e3") == +67128.9;
//        assert std.numeric.parse_real("-0b01000011.00100001e3") == -67128.9;
//        assert std.numeric.parse_real(" 0x43.21  ") == +67.1289;
//        assert std.numeric.parse_real("+0x43.21  ") == +67.1289;
//        assert std.numeric.parse_real("-0x43.21  ") == -67.1289;
//        assert std.numeric.parse_real(" 0x43.21p3") == +67.1289;
//        assert std.numeric.parse_real("+0x43.21p3") == +67.1289;
//        assert std.numeric.parse_real("-0x43.21p3") == -67.1289;
//        assert std.numeric.parse_real(" 0x43.21e3") == +67128.9;
//        assert std.numeric.parse_real("+0x43.21e3") == +67128.9;
//        assert std.numeric.parse_real("-0x43.21e3") == -67128.9;
//        assert std.numeric.parse_real(" 1e1000000") == null;
//        assert std.numeric.parse_real("+1e1000000") == null;
//        assert std.numeric.parse_real("-1e1000000") == null;
//        assert std.numeric.parse_real(" 1e1000000", true) ==  +infinity;
//        assert std.numeric.parse_real("+1e1000000", true) ==  +infinity;
//        assert std.numeric.parse_real("-1e1000000", true) ==  -infinity;
//        assert std.numeric.parse_real(" 0e1000000") == 0;
//        assert std.numeric.parse_real("+0e1000000") == 0;
//        assert std.numeric.parse_real("-0e1000000") == 0;
      )__";

    std::istringstream iss(s_source);
    Simple_Source_File code(iss, rocket::sref("my_file"));
    Global_Context global;
    code.execute(global, { });
  }
