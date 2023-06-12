#pragma once

extern volatile int __entrypoint_r13;
extern volatile int __entrypoint_r14;
extern volatile int __entrypoint_r15;

__attribute__((always_inline))
inline void __entrypoint_capture()
{
	asm volatile ("mov %0, r13" : "=r"(__entrypoint_r13));
	asm volatile ("mov %0, r14" : "=r"(__entrypoint_r14));
	asm volatile ("mov %0, r15" : "=r"(__entrypoint_r15));
	asm volatile ("nop");
	
}

inline void __entrypoint_return()
{
	asm volatile ("mov r13, %0" :: "r"(__entrypoint_r13));
	asm volatile ("mov r14, %0" :: "r"(__entrypoint_r14));
	asm volatile ("mov r15, %0" :: "r"(__entrypoint_r15));	
}

