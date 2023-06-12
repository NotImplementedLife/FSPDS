#pragma once

namespace DSC
{	
	extern bool const IS_TESTMOD;
	
	void testmod_fatal_raise();
	bool testmod_fatal_raised();
	void testmod_reset();
	
	extern volatile int __testmod_r13;
	extern volatile int __testmod_r14;
	extern volatile int __testmod_r15;

	__attribute__((always_inline))
	inline void testmod_capture()
	{
		asm volatile ("mov %0, r13" : "=r"(__testmod_r13));
		asm volatile ("mov %0, r14" : "=r"(__testmod_r14));
		asm volatile ("mov %0, r15" : "=r"(__testmod_r15));
		asm volatile ("nop");
		
	}

	inline void testmod_return()
	{
		asm volatile ("mov r13, %0" :: "r"(__testmod_r13));
		asm volatile ("mov r14, %0" :: "r"(__testmod_r14));
		asm volatile ("mov r15, %0" :: "r"(__testmod_r15));	
	}
	
	typedef void (*TestHandler)();
	
	void testmod_load_next();
	void __testmod_register__(TestHandler test_handler, const char* name, bool expect_fatal = false);
		
	
	#define _testmod_register_1(handler) DSC::__testmod_register__((handler), (#handler))
	#define _testmod_register_2(handler, expect_fatal) DSC::__testmod_register__((handler), (#handler), (expect_fatal))

	#define _ARG2(_0, _1, _2, ...) _2
	#define NARG2(...) _ARG2(__VA_ARGS__, 2, 1, 0)

	#define __testmod_register_N(N, ...) _testmod_register_ ## N (__VA_ARGS__)
	#define _testmod_register_N(N, ...) __testmod_register_N(N, __VA_ARGS__)
	#define testmod_register(...) _testmod_register_N(NARG2(__VA_ARGS__), __VA_ARGS__)

	
	bool testmod_ready();
	void testmod_run();
	bool testmod_expected_fatal();
	
	void testmod_report_success();
	void testmod_report_fail();
	
	void testmod_terminate();
	
	extern void testmod_init();	
}
