#include <unordered_set>
#include <string>
#include <iostream>
#include <dlfcn.h>

static std::unordered_set<std::string> missingFuncs;

void RedirectLuaProviderEntries(void* from, void* to, void* refer) {
	
}

class Bridge {
public:
    Bridge() {
		std::cout << "DontStarveLuajit initialized!" << std::endl;
		void* from = dlopen(nullptr, RTLD_GLOBAL);
		void* refer = dlopen("liblua51DS.so", RTLD_GLOBAL | RTLD_NOW);
		void* to = dlopen("liblua51.so", RTLD_GLOBAL | RTLD_NOW);
		std::cout << "Main address: " << std::hex << from << std::endl;
		std::cout << "Lua51 address: " << std::hex << refer << std::endl;
		std::cout << "LuaJIT address: " << std::hex << to << std::endl;

		const char* funcs[] = {
			"luaL_addlstring", "luaL_addstring", "luaL_addvalue", "luaL_argerror",
			"luaL_buffinit", "luaL_callmeta", "luaL_checkany", "luaL_checkinteger",
			"luaL_checklstring", "luaL_checknumber", "luaL_checkoption", "luaL_checkstack",
			"luaL_checkudata", "luaL_findtable", "luaL_getmetafield", "luaL_gsub",
			"luaL_loadbuffer", "luaL_loadfile", "luaL_loadstring", "luaL_newmetatable",
			"luaL_newstate", "luaL_openlib", "luaL_openlibs", "luaL_optinteger",
			"luaL_optlstring", "luaL_optnumber", "luaL_prepbuffer", "luaL_pushresult",
			"luaL_ref", "luaL_register", "luaL_unref", "luaL_where",
			"lua_cpcall", "lua_equal", "lua_getallocf", "lua_gethook",
			"lua_gethookcount", "lua_gethookmask", "lua_getupvalue", "lua_isuserdata",
			"lua_pushvfstring", "lua_setallocf", "lua_setupvalue", "lua_status",
		};

		for (size_t i = 0; i < sizeof(funcs) / sizeof(funcs[0]); i++) {
			missingFuncs.insert(funcs[i]);
		}
    }
} theBridge;