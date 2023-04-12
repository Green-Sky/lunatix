#include "./tox_lua_module.hpp"

#include <solanaceae/core/tox_interface.hpp>

#include <luacode.h>
#include <LuaBridge3/LuaBridge.h>

#include <fstream>

#define REG_ENUM(x) template<> struct luabridge::Stack<x> : luabridge::Enum<x> {};

REG_ENUM(Tox_Connection)
REG_ENUM(Tox_User_Status)
REG_ENUM(Tox_Message_Type)
REG_ENUM(Tox_File_Control)
REG_ENUM(Tox_Conference_Type)
REG_ENUM(Tox_Group_Privacy_State)
REG_ENUM(Tox_Group_Role)
REG_ENUM(Tox_Group_Voice_State)
REG_ENUM(Tox_Group_Topic_Lock)

REG_ENUM(Tox_Err_Set_Info)
REG_ENUM(Tox_Err_Friend_Add)
REG_ENUM(Tox_Err_Friend_Delete)
REG_ENUM(Tox_Err_Friend_Get_Public_Key)
REG_ENUM(Tox_Err_Friend_Send_Message)
REG_ENUM(Tox_Err_File_Control)
REG_ENUM(Tox_Err_File_Seek)
REG_ENUM(Tox_Err_File_Get)
REG_ENUM(Tox_Err_File_Send)
REG_ENUM(Tox_Err_File_Send_Chunk)
REG_ENUM(Tox_Err_Conference_Delete)
REG_ENUM(Tox_Err_Conference_Peer_Query)
REG_ENUM(Tox_Err_Conference_Set_Max_Offline)
REG_ENUM(Tox_Err_Conference_Invite)
REG_ENUM(Tox_Err_Conference_Join)
REG_ENUM(Tox_Err_Conference_Send_Message)
REG_ENUM(Tox_Err_Conference_Title)
REG_ENUM(Tox_Err_Conference_Get_Type)
REG_ENUM(Tox_Err_Conference_By_Id)
REG_ENUM(Tox_Err_Conference_By_Uid)
REG_ENUM(Tox_Err_Friend_Custom_Packet)
REG_ENUM(Tox_Err_Group_New)
REG_ENUM(Tox_Err_Group_Join)
REG_ENUM(Tox_Err_Group_Disconnect)
REG_ENUM(Tox_Err_Group_Reconnect)
REG_ENUM(Tox_Err_Group_Leave)
REG_ENUM(Tox_Err_Group_Self_Name_Set)
REG_ENUM(Tox_Err_Group_Self_Status_Set)
REG_ENUM(Tox_Err_Group_Self_Query)
REG_ENUM(Tox_Err_Group_Peer_Query)
REG_ENUM(Tox_Err_Group_Topic_Set)
REG_ENUM(Tox_Err_Group_State_Queries)
REG_ENUM(Tox_Err_Group_Send_Message)
REG_ENUM(Tox_Err_Group_Send_Private_Message)
REG_ENUM(Tox_Err_Group_Send_Custom_Packet)
REG_ENUM(Tox_Err_Group_Send_Custom_Private_Packet)
REG_ENUM(Tox_Err_Group_Invite_Friend)
REG_ENUM(Tox_Err_Group_Invite_Accept)
REG_ENUM(Tox_Err_Group_Founder_Set_Password)
REG_ENUM(Tox_Err_Group_Founder_Set_Topic_Lock)
REG_ENUM(Tox_Err_Group_Founder_Set_Voice_State)
REG_ENUM(Tox_Err_Group_Founder_Set_Privacy_State)
REG_ENUM(Tox_Err_Group_Founder_Set_Peer_Limit)
REG_ENUM(Tox_Err_Group_Set_Ignore)
REG_ENUM(Tox_Err_Group_Mod_Set_Role)
REG_ENUM(Tox_Err_Group_Mod_Kick_Peer)

ToxLuaModule::ToxLuaModule(ToxI& t, ToxEventProviderI& tep) : _t(t) {
	auto* L = _lua_state_global.get();
	{ // setup global lua state
		luaL_openlibs(L);

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

		luabridge::getGlobalNamespace(L)
		.beginNamespace("test")
			.beginClass<ToxI>("Tox")
				//.addFunction("scale", &scale)
				.addFunction("toxSelfGetConnectionStatus", &ToxI::toxSelfGetConnectionStatus)
				.addFunction("toxSelfGetAddressStr", &ToxI::toxSelfGetAddressStr)
			.endClass()
		.endNamespace()
		;

		luabridge::push(L, &_t);
		lua_setglobal(L, "TOX");
	}

	{ // start lua
		std::ifstream ifile{"main.lua", std::ios::binary};

		if (!ifile.is_open()) {
			std::cerr << "TLM missing main.lua\n";
			exit(1);
		}

		std::cout << "TLM loading main.lua\n";
		std::string lua_main_code;
		while (ifile.good()) {
			auto ch = ifile.get();
			if (ch == EOF) {
				break;
			} else {
				lua_main_code.push_back(ch); // TODO: this is slow
			}
		}

		// load lua
		size_t byte_code_size = 0;
		std::unique_ptr<char, void(*)(void*)> byte_code {
			luau_compile(lua_main_code.data(), lua_main_code.size(), nullptr, &byte_code_size),
			std::free
		};
		// TODO: error handling
		assert(byte_code);

		// execute lua
		luau_load(L, "main.lua", byte_code.get(), byte_code_size, 0);
		lua_call(L, 0, 0);
		// once executed, can we free it? (unique_ptr frees here)
	}

	// TODO: all?
	tep.subscribe(this, Tox_Event::TOX_EVENT_FRIEND_MESSAGE);
	tep.subscribe(this, Tox_Event::TOX_EVENT_GROUP_MESSAGE);
}

ToxLuaModule::~ToxLuaModule(void) {
}

