#include "test_easy_profiler.h"

#include <easy/profiler.h>
#include <iostream>
#include "util/file_helper.h"

void foo()
{
	EASY_FUNCTION(profiler::colors::Magenta);  // Magenta block with name "foo"

	EASY_BLOCK("Calculating sum");	// Begin block with default color == Amber100
	int sum = 0;
	for (int i = 0; i < 10; ++i)
	{
		EASY_BLOCK(
			"Addition",
			profiler::colors::Red
		);	// Scoped red block (no EASY_END_BLOCK needed)
		sum += i;
	}
	EASY_END_BLOCK;	 // End of "Calculating sum" block

	EASY_BLOCK("Calculating multiplication", profiler::colors::Blue500);  // Blue block
	int mul = 1;
	for (int i = 1; i < 11; ++i)
		mul *= i;
	// EASY_END_BLOCK; // This is not needed because all blocks are ended on destructor when closing
	// braces met
}

void bar()
{
	EASY_FUNCTION(0xfff080aa);	// Function block with custom ARGB color
}

void baz()
{
	EASY_FUNCTION();  // Function block with default color == Amber100
}

int test_easy_profiler()
{
	EASY_PROFILER_ENABLE;
	profiler::startListen();

	foo();

	char profPath[200];
	snprintf(profPath, 199, "%s/test_profile.prof", scgf::util::get_executable_dir());
	std::cout << "prof file path: " << profPath << std::endl;
	profiler::dumpBlocksToFile(profPath);
	return 0;
}