#include "LuaHelper.h"

#include "Compressor.h"
#include "Cryptor.h"
#include "File.h"
#include <iostream>
#include "Logger.h"
#include "String.h"

namespace scgf::util
{
LuaHelper::LuaHelper()
	: m_luaState(nullptr)
{
	m_luaState = luaL_newstate();
	luaL_openlibs(m_luaState);

	register_functions();
}

LuaHelper::~LuaHelper() { lua_close(m_luaState); }

void LuaHelper::register_functions()
{
	lua_register(m_luaState, "scgf_log_info", &LuaHelper::log_info);
	lua_register(m_luaState, "scgf_add", &LuaHelper::add);
	lua_register(m_luaState, "scgf_get_project_base_dir", &LuaHelper::get_project_base_dir);
	lua_register(m_luaState, "scgf_get_now_time_ms", &LuaHelper::get_now_time_ms);
}

bool LuaHelper::run_lua_file(const char* filePath, bool isSCGF)
{
	SCGF_LOG_INFO("Running lua script: {}...", filePath);

	if (isSCGF)
	{
		auto [fileData, fileDataSize] = read_file(filePath);

		auto [decryptedData, decryptedSize] =
			Cryptor::get_instance().decrypt_scgf(fileData.get(), fileDataSize);

		auto [decompressedData, decompressedSize] =
			Compressor::get_instance().decompress(decryptedData.get(), decryptedSize);

		std::string xmlString(decompressedData.get());

		replace(xmlString, "\r\n", "");

		if (luaL_dostring(m_luaState, xmlString.c_str()) != LUA_OK)
		{
			SCGF_LOG_ERROR(lua_tostring(m_luaState, -1));
			lua_pop(m_luaState, 1);
			return false;
		}
		return true;
	}

	if (luaL_dofile(m_luaState, filePath) != LUA_OK)
	{
		SCGF_LOG_ERROR(lua_tostring(m_luaState, -1));
		lua_pop(m_luaState, 1);	 // Erase error msg
		return false;
	}
	return true;
}

int LuaHelper::log_info(lua_State* L)
{
	const char* message = lua_tostring(L, 1);
	SCGF_LOG_INFO(message);
	return 0;  // 没有返回值
}

int LuaHelper::add(lua_State* L)
{
	double a = lua_tonumber(L, 1);
	double b = lua_tonumber(L, 2);
	lua_pushnumber(L, a + b);
	return 1;  // 返回一个值
}

int LuaHelper::get_now_time_ms(lua_State* L)
{
	time_t now = time(0);
	lua_pushinteger(L, now);
	return 1;  // 返回一个值
}

int LuaHelper::get_project_base_dir(lua_State* L)
{
	lua_pushstring(L, PROJECT_BASE_DIR);
	return 1;  // 返回一个值
}
}  // namespace scgf::util