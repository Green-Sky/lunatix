#pragma once

#include <solanaceae/toxcore/tox_event_interface.hpp>

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

	public:
		void iterate(void);

	protected: // tox events

#define OVER_EVENT(x) bool onToxEvent(const x*) override;

	OVER_EVENT(Tox_Event_Conference_Connected)
	OVER_EVENT(Tox_Event_Conference_Invite)
	OVER_EVENT(Tox_Event_Conference_Message)
	OVER_EVENT(Tox_Event_Conference_Peer_List_Changed)
	OVER_EVENT(Tox_Event_Conference_Peer_Name)
	OVER_EVENT(Tox_Event_Conference_Title)

	OVER_EVENT(Tox_Event_File_Chunk_Request)
	OVER_EVENT(Tox_Event_File_Recv)
	OVER_EVENT(Tox_Event_File_Recv_Chunk)
	OVER_EVENT(Tox_Event_File_Recv_Control)

	OVER_EVENT(Tox_Event_Friend_Connection_Status)
	OVER_EVENT(Tox_Event_Friend_Lossless_Packet)
	OVER_EVENT(Tox_Event_Friend_Lossy_Packet)
	OVER_EVENT(Tox_Event_Friend_Message)
	OVER_EVENT(Tox_Event_Friend_Name)
	OVER_EVENT(Tox_Event_Friend_Read_Receipt)
	OVER_EVENT(Tox_Event_Friend_Request)
	OVER_EVENT(Tox_Event_Friend_Status)
	OVER_EVENT(Tox_Event_Friend_Status_Message)
	OVER_EVENT(Tox_Event_Friend_Typing)

	OVER_EVENT(Tox_Event_Self_Connection_Status)

	OVER_EVENT(Tox_Event_Group_Peer_Name)
	OVER_EVENT(Tox_Event_Group_Peer_Status)
	OVER_EVENT(Tox_Event_Group_Topic)
	OVER_EVENT(Tox_Event_Group_Privacy_State)
	OVER_EVENT(Tox_Event_Group_Voice_State)
	OVER_EVENT(Tox_Event_Group_Topic_Lock)
	OVER_EVENT(Tox_Event_Group_Peer_Limit)
	OVER_EVENT(Tox_Event_Group_Password)
	OVER_EVENT(Tox_Event_Group_Message)
	OVER_EVENT(Tox_Event_Group_Private_Message)
	OVER_EVENT(Tox_Event_Group_Custom_Packet)
	OVER_EVENT(Tox_Event_Group_Custom_Private_Packet)
	OVER_EVENT(Tox_Event_Group_Invite)
	OVER_EVENT(Tox_Event_Group_Peer_Join)
	OVER_EVENT(Tox_Event_Group_Peer_Exit)
	OVER_EVENT(Tox_Event_Group_Self_Join)
	OVER_EVENT(Tox_Event_Group_Join_Fail)
	OVER_EVENT(Tox_Event_Group_Moderation)

#undef OVER_EVENT

};

