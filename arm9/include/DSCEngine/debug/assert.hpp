#pragma once


//https://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
#define _nds_assert_1(expr) DSC::__assert__((expr), (#expr))
#define _nds_assert_2(expr, msg) DSC::__assert__((expr), (msg))

#define _ARG2(_0, _1, _2, ...) _2
#define NARG2(...) _ARG2(__VA_ARGS__, 2, 1, 0)

#define __nds_assert_N(N, ...) _nds_assert_ ## N (__VA_ARGS__)
#define _nds_assert_N(N, ...) __nds_assert_N(N, __VA_ARGS__)

// calling it nds_assert to differentiate it from stdlib's assert
#define nds_assert(...) _nds_assert_N(NARG2(__VA_ARGS__), __VA_ARGS__)

// usage :
// nds_assert(condition == ?);
// nds_assert(condition == ?, "Fatal message if failed");


namespace DSC
{
	void __assert__(bool condition, const char* message);
}