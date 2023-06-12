#include "DSCEngine/testmod.hpp"
#include "DSCEngine/debug/log.hpp"
#include "DSCEngine/types/vector.hpp"

#if TESTMOD
	bool const DSC::IS_TESTMOD = true;
	#warning TESTMOD activated
#else
	bool const DSC::IS_TESTMOD = false;
	#warning TESTMOD not activated
#endif

namespace
{
	bool raised_fatal = false;	
	DSC::Vector<DSC::TestHandler> tests;
	DSC::Vector<bool> efatal;
	DSC::Vector<const char*> test_name;
	int current_test = -1;
	int passed = 0;
}

volatile int DSC::__testmod_r13 = 0;
volatile int DSC::__testmod_r14 = 0;
volatile int DSC::__testmod_r15 = 0;

void DSC::testmod_fatal_raise() { raised_fatal = true; }
bool DSC::testmod_fatal_raised() { return raised_fatal; }
void DSC::testmod_reset()
{
	raised_fatal = false;
}

void DSC::testmod_load_next()
{
	current_test++;
}

void DSC::testmod_run()
{
	tests[current_test]();
}

bool DSC::testmod_ready()
{
	return current_test == tests.size()-1;
}

bool DSC::testmod_expected_fatal()
{
	return efatal[current_test];
}

void DSC::__testmod_register__(DSC::TestHandler test_handler, const char* name, bool expect_fatal)
{
	tests.push_back(test_handler);
	efatal.push_back(expect_fatal);
	test_name.push_back(name);
}

void DSC::testmod_report_success()
{
	Debug::log("# Passed \"%s\" (%i)", test_name[current_test], current_test+1);
	passed++;
}

void DSC::testmod_report_fail()
{
	Debug::log("# Failed \"%s\" (%i)", test_name[current_test], current_test+1);
}

void DSC::testmod_terminate()
{
	Debug::log("Test Module terminated");
	Debug::log("Passed %i tests out of %i", passed, tests.size());
}