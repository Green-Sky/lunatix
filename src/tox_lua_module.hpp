#pragma once

#include <solanaceae/core/tox_event_interface.hpp>

#include <lua.h>
#include <lualib.h>

#include <memory>

// fwd
struct ToxI;

class ToxLuaModule : public ToxEventI {
	ToxI& _t;

	std::unique_ptr<lua_State, void(*)(lua_State*)> _lua_state_global {luaL_newstate(), lua_close};

	public:
		ToxLuaModule(ToxI& t, ToxEventProviderI& tep);
		~ToxLuaModule(void);

};

