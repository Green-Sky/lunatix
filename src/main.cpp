#include <tox/tox.h>

#include <luacode.h>
#include <lua.h>
#include <lualib.h>

#include "./tox_client.hpp"

#include <string_view>
#include <iostream>
#include <thread>
#include <cassert>
#include <cstdlib>

// https://youtu.be/YIE4b8gT3ho

int main(void) {
	std::string_view test_code {R"(
function ispositive(x)
	return x > 0
end

print(ispositive(1))
)"};

	char* byte_code = nullptr;
	size_t byte_code_size = 0;

	// load lua
	byte_code = luau_compile(test_code.data(), test_code.size(), nullptr, &byte_code_size);
	// TODO: error handling
	assert(byte_code);
	std::free(byte_code);

	ToxClient tcl;

	//luaL_openlibs(L);

	{ // add global functions
		//static const luaL_Reg funcs[] = {
			//{"loadstring", lua_loadstring},
			//{"require", lua_require},
			//{NULL, NULL},
		//};

		//lua_pushvalue(L, LUA_GLOBALSINDEX);
		//luaL_register(L, NULL, funcs);
		//lua_pop(L, 1);
	}

	//// execute lua
	//luau_load(L, "main", byte_code, byte_code_size, 0);
	//lua_call(L, 0, 0);

	std::cout << "tox id: " << tcl.getOwnAddress() << "\n";

	while (tcl.iterate()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	return 0;
}

