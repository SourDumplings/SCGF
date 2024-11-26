#pragma once

#include <lua.hpp>
#include "singleton.h"

namespace scgf::util
{
class CLuaHelper final : public CSingleton<CLuaHelper>
{
public:
	CLuaHelper();
	virtual ~CLuaHelper();

	bool run_lua_file(const char* filePath, bool isSCGF = true);

private:
	lua_State* m_luaState;

	/* Lua APIs Begin */
	static int log_info(lua_State* L);
	static int add(lua_State* L);
	static int get_now_time_ms(lua_State* L);
	static int get_project_base_dir(lua_State* L);
	/* Lua APIs End */

	void register_functions();
};

}  // namespace scgf::util