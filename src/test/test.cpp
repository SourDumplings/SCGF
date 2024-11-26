#include "test.h"
#include "test_easy_profiler.h"
#include "test_ecs.h"
#include "test_entt.h"
#include "test_imgui.h"
#include "test_litehtml.h"
#include "test_lua.h"
#include "test_pb.h"
#include "test_scgf_xml.h"
#include "test_sdl3.h"
#include "test_spdlog.h"
#include "test_string.h"
#include <cstdlib>
#include <iostream>

void test_asan()
{
	int x[100];
	x[100] = 5;	 // Boom!
}

int test()
{
	// test_asan();
	// test_easy_profiler();
	// test_ecs();
	// test_entt();
	// test_imgui();
	// test_litehtml();
	// test_lua();
	// test_pb();
	// test_scgf_xml();
	test_sdl3();
	// test_spdlog();
	// test_string();
	return 0;
}