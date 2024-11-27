#include "test_lua.h"

#include <filesystem>
#include <iostream>
#include <lua.hpp>
#include "util/lua_helper.h"
#include "util/file_helper.h"

int simple_test_lua()
{
	// 初始化 Lua 解释器
	lua_State* L = luaL_newstate();
	luaL_openlibs(L);

	// Lua 代码片段
	const char* luaCode = "print('Hello')";

	// 打印 Lua 代码片段
	std::cout << "Lua Code: " << luaCode << std::endl;

	// 执行 Lua 字符串
	if (luaL_dostring(L, luaCode) != LUA_OK)
	{
		std::cerr << "Error: " << lua_tostring(L, -1) << std::endl;
		lua_pop(L, 1);	// 移除错误消息
	}

	// 关闭 Lua 解释器
	lua_close(L);

	return 0;
}

int test_script()
{
	std::filesystem::path cwd			 = scgf::util::get_executable_dir();
	std::filesystem::path projectBaseDir = PROJECT_BASE_DIR;

	std::cout << "Running raw lua script..." << std::endl;
	std::filesystem::path rawLuaPath = "res/script/test.lua";
	rawLuaPath						 = projectBaseDir / rawLuaPath;

	if (std::filesystem::exists(rawLuaPath))
	{
		std::cout << "rawLuaPath exists: " << rawLuaPath << std::endl;
	}
	else { std::cout << "rawLuaPath does not exist: " << rawLuaPath << std::endl; }

	scgf::util::CLuaHelper::get_instance().run_lua_file(rawLuaPath.string().c_str(), false);

	std::cout << "Running scgf lua script..." << std::endl;
	std::filesystem::path scgfLuaPath = "../res/dist/scgf/script/test.scgf";
	scgfLuaPath						  = cwd / scgfLuaPath;
	scgf::util::CLuaHelper::get_instance().run_lua_file(scgfLuaPath.string().c_str());

	return 0;
}

int test_lua()
{
	// simple_test_lua();
	test_script();

	return 0;
}