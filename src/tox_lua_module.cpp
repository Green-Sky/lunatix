#include "./tox_lua_module.hpp"

#include <solanaceae/core/tox_interface.hpp>

#include <luacode.h>
#include <LuaBridge3/LuaBridge.h>

#include <fstream>
#include <type_traits>

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
		.beginNamespace("tox")
			.beginClass<ToxI>("Tox")
				// TODO: keep them all up to date (aka delete and renew once in a while)
				.addFunction("toxSelfGetConnectionStatus", &ToxI::toxSelfGetConnectionStatus)
				.addFunction("toxIterationInterval", &ToxI::toxIterationInterval)
				.addFunction("toxSelfGetAddress", &ToxI::toxSelfGetAddress)
				.addFunction("toxSelfGetAddressStr", &ToxI::toxSelfGetAddressStr)
				.addFunction("toxSelfSetNospam", &ToxI::toxSelfSetNospam)
				.addFunction("toxSelfGetNospam", &ToxI::toxSelfGetNospam)
				.addFunction("toxSelfGetPublicKey", &ToxI::toxSelfGetPublicKey)
				.addFunction("toxSelfSetName", &ToxI::toxSelfSetName)
				.addFunction("toxSelfGetName", &ToxI::toxSelfGetName)
				.addFunction("toxSelfSetStatusMessage", &ToxI::toxSelfSetStatusMessage)
				.addFunction("toxSelfGetStatusMessage", &ToxI::toxSelfGetStatusMessage)
				.addFunction("toxSelfSetStatus", &ToxI::toxSelfSetStatus)
				.addFunction("toxSelfGetStatus", &ToxI::toxSelfGetStatus)
				.addFunction("toxFriendAdd", &ToxI::toxFriendAdd)
				.addFunction("toxFriendAddNorequest", &ToxI::toxFriendAddNorequest)
				.addFunction("toxFriendDelete", &ToxI::toxFriendDelete)
				.addFunction("toxFriendGetPublicKey", &ToxI::toxFriendGetPublicKey)
				.addFunction("toxFriendSendMessage", &ToxI::toxFriendSendMessage)
				.addFunction("toxHash", &ToxI::toxHash)
				.addFunction("toxFileControl", &ToxI::toxFileControl)
				.addFunction("toxFileSeek", &ToxI::toxFileSeek)
				.addFunction("toxFileGetFileID", &ToxI::toxFileGetFileID)
				.addFunction("toxFileSend", &ToxI::toxFileSend)
				.addFunction("toxFileSendChunk", &ToxI::toxFileSendChunk)
				.addFunction("toxConferenceJoin", &ToxI::toxConferenceJoin)
				.addFunction("toxConferenceSendMessage", &ToxI::toxConferenceSendMessage)
				.addFunction("toxFriendSendLossyPacket", &ToxI::toxFriendSendLossyPacket)
				.addFunction("toxFriendSendLosslessPacket", &ToxI::toxFriendSendLosslessPacket)
				.addFunction("toxGroupNew", &ToxI::toxGroupNew)
				.addFunction("toxGroupJoin", &ToxI::toxGroupJoin)
				.addFunction("toxGroupIsConnected", &ToxI::toxGroupIsConnected)
				.addFunction("toxGroupReconnect", &ToxI::toxGroupReconnect)
				.addFunction("toxGroupLeave", &ToxI::toxGroupLeave)
				.addFunction("toxGroupSelfGetName", &ToxI::toxGroupSelfGetName)
				.addFunction("toxGroupPeerGetName", &ToxI::toxGroupPeerGetName)
				.addFunction("toxGroupPeerGetConnectionStatus", &ToxI::toxGroupPeerGetConnectionStatus)
				.addFunction("toxGroupSetTopic", &ToxI::toxGroupSetTopic)
				.addFunction("toxGroupGetTopic", &ToxI::toxGroupGetTopic)
				.addFunction("toxGroupGetName", &ToxI::toxGroupGetName)
				.addFunction("toxGroupGetChatId", &ToxI::toxGroupGetChatId)
				.addFunction("toxGroupSendMessage", &ToxI::toxGroupSendMessage)
				.addFunction("toxGroupSendPrivateMessage", &ToxI::toxGroupSendPrivateMessage)
				.addFunction("toxGroupSendCustomPacket", &ToxI::toxGroupSendCustomPacket)
				.addFunction("toxGroupSendCustomPrivatePacket", &ToxI::toxGroupSendCustomPrivatePacket)
				.addFunction("toxGroupInviteFriend", &ToxI::toxGroupInviteFriend)
				.addFunction("toxGroupInviteAccept", &ToxI::toxGroupInviteAccept)
			.endClass()
		.endNamespace();

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

	tep.subscribe(this, TOX_EVENT_SELF_CONNECTION_STATUS);

	tep.subscribe(this, TOX_EVENT_FRIEND_REQUEST);
	tep.subscribe(this, TOX_EVENT_FRIEND_CONNECTION_STATUS);
	tep.subscribe(this, TOX_EVENT_FRIEND_LOSSY_PACKET);
	tep.subscribe(this, TOX_EVENT_FRIEND_LOSSLESS_PACKET);

	tep.subscribe(this, TOX_EVENT_FRIEND_NAME);
	tep.subscribe(this, TOX_EVENT_FRIEND_STATUS);
	tep.subscribe(this, TOX_EVENT_FRIEND_STATUS_MESSAGE);

	tep.subscribe(this, TOX_EVENT_FRIEND_MESSAGE);
	tep.subscribe(this, TOX_EVENT_FRIEND_READ_RECEIPT);
	tep.subscribe(this, TOX_EVENT_FRIEND_TYPING);

	tep.subscribe(this, TOX_EVENT_FILE_CHUNK_REQUEST);
	tep.subscribe(this, TOX_EVENT_FILE_RECV);
	tep.subscribe(this, TOX_EVENT_FILE_RECV_CHUNK);
	tep.subscribe(this, TOX_EVENT_FILE_RECV_CONTROL);

	tep.subscribe(this, TOX_EVENT_CONFERENCE_INVITE);
	tep.subscribe(this, TOX_EVENT_CONFERENCE_CONNECTED);
	tep.subscribe(this, TOX_EVENT_CONFERENCE_PEER_LIST_CHANGED);
	tep.subscribe(this, TOX_EVENT_CONFERENCE_PEER_NAME);
	tep.subscribe(this, TOX_EVENT_CONFERENCE_TITLE);

	tep.subscribe(this, TOX_EVENT_CONFERENCE_MESSAGE);

	tep.subscribe(this, TOX_EVENT_GROUP_PEER_NAME);
	tep.subscribe(this, TOX_EVENT_GROUP_PEER_STATUS);
	tep.subscribe(this, TOX_EVENT_GROUP_TOPIC);
	tep.subscribe(this, TOX_EVENT_GROUP_PRIVACY_STATE);
	tep.subscribe(this, TOX_EVENT_GROUP_VOICE_STATE);
	tep.subscribe(this, TOX_EVENT_GROUP_TOPIC_LOCK);
	tep.subscribe(this, TOX_EVENT_GROUP_PEER_LIMIT);
	tep.subscribe(this, TOX_EVENT_GROUP_PASSWORD);
	tep.subscribe(this, TOX_EVENT_GROUP_MESSAGE);
	tep.subscribe(this, TOX_EVENT_GROUP_PRIVATE_MESSAGE);
	tep.subscribe(this, TOX_EVENT_GROUP_CUSTOM_PACKET);
	tep.subscribe(this, TOX_EVENT_GROUP_CUSTOM_PRIVATE_PACKET);
	tep.subscribe(this, TOX_EVENT_GROUP_INVITE);
	tep.subscribe(this, TOX_EVENT_GROUP_PEER_JOIN);
	tep.subscribe(this, TOX_EVENT_GROUP_PEER_EXIT);
	tep.subscribe(this, TOX_EVENT_GROUP_SELF_JOIN);
	tep.subscribe(this, TOX_EVENT_GROUP_JOIN_FAIL);
	tep.subscribe(this, TOX_EVENT_GROUP_MODERATION);
}

ToxLuaModule::~ToxLuaModule(void) {
}

#if 0
template<typename EventT, typename FN>
auto callEventArgs(const EventT* e, FN&& fn) {
	if constexpr (std::is_same_v<EventT, Tox_Event_Self_Connection_Status>) {
		return fn(
			tox_event_self_connection_status_get_connection_status(e)
		);
	} // else if ...
}

#else
//template<typename EventT, typename FN>
//auto callEventArgs(const EventT* e, FN&& fn) { return fn(); }

template<typename FN>
auto callEventArgs(const Tox_Event_Self_Connection_Status* e, FN&& fn) {
	return fn(
		tox_event_self_connection_status_get_connection_status(e)
	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Conference_Connected* e, FN&& fn) {
	return fn(
		tox_event_conference_connected_get_conference_number(e)
	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Conference_Invite* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Conference_Message* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Conference_Peer_List_Changed* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Conference_Peer_Name* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Conference_Title* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_File_Chunk_Request* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_File_Recv* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_File_Recv_Chunk* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_File_Recv_Control* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Connection_Status* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Lossless_Packet* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Lossy_Packet* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Message* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Name* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Read_Receipt* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Request* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Status* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Status_Message* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Friend_Typing* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Peer_Name* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Peer_Status* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Topic* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Privacy_State* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Voice_State* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Topic_Lock* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Peer_Limit* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Password* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Message* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Private_Message* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Custom_Packet* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Custom_Private_Packet* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Invite* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Peer_Join* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Peer_Exit* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Self_Join* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Join_Fail* e, FN&& fn) {
	return fn(

	);
}

template<typename FN>
auto callEventArgs(const Tox_Event_Group_Moderation* e, FN&& fn) {
	return fn(

	);
}

#endif

bool ToxLuaModule::onToxEvent(const Tox_Event_Self_Connection_Status* e) {
	// get lua function
	luabridge::LuaRef g_events_table = luabridge::getGlobal(_lua_state_global.get(), "TOX_EVENTS");
	if (!g_events_table.isTable()) {
		std::cerr << "TLM waring: global table TOX_EVENTS not set\n";
		return false;
	}

	auto lr_fn = g_events_table["Tox_Event_Self_Connection_Status"];
	if (!lr_fn.isCallable()) {
		std::cerr << "TLM waring: Tox_Event_Self_Connection_Status is not callable\n";
		return false;
	}

	// call
	auto res = callEventArgs(e, lr_fn);

	if (res.hasFailed() || res.size() != 1) {
		std::cerr << "TLM error, Tox_Event_Self_Connection_Status callback failed\n";
		return false;
	}

	if (!res[0].isBool()) {
		std::cerr << "TLM error, Tox_Event_Self_Connection_Status callback did not return a bool\n";
		return false;
	}

	return static_cast<bool>(res[0]);
}

#define EVENT_IMPL(x) \
bool ToxLuaModule::onToxEvent(const x* e) { \
	luabridge::LuaRef g_events_table = luabridge::getGlobal(_lua_state_global.get(), "TOX_EVENTS"); \
	if (!g_events_table.isTable()) { \
		std::cerr << "TLM waring: global table TOX_EVENTS not set\n"; \
		return false; \
	} \
	auto lr_fn = g_events_table[#x]; \
	if (!lr_fn.isCallable()) { \
		return false; \
	} \
	auto res = callEventArgs(e, lr_fn); \
	if (res.hasFailed() || res.size() != 1) { \
		std::cerr << "TLM error, " #x " callback failed\n"; \
		return false; \
	} \
	if (!res[0].isBool()) { \
		std::cerr << "TLM error, " #x " callback did not return a bool\n"; \
		return false; \
	} \
	return static_cast<bool>(res[0]); \
}

EVENT_IMPL(Tox_Event_Conference_Connected)
EVENT_IMPL(Tox_Event_Conference_Invite)
EVENT_IMPL(Tox_Event_Conference_Message)
EVENT_IMPL(Tox_Event_Conference_Peer_List_Changed)
EVENT_IMPL(Tox_Event_Conference_Peer_Name)
EVENT_IMPL(Tox_Event_Conference_Title)

EVENT_IMPL(Tox_Event_File_Chunk_Request)
EVENT_IMPL(Tox_Event_File_Recv)
EVENT_IMPL(Tox_Event_File_Recv_Chunk)
EVENT_IMPL(Tox_Event_File_Recv_Control)

EVENT_IMPL(Tox_Event_Friend_Connection_Status)
EVENT_IMPL(Tox_Event_Friend_Lossless_Packet)
EVENT_IMPL(Tox_Event_Friend_Lossy_Packet)
EVENT_IMPL(Tox_Event_Friend_Message)
EVENT_IMPL(Tox_Event_Friend_Name)
EVENT_IMPL(Tox_Event_Friend_Read_Receipt)
EVENT_IMPL(Tox_Event_Friend_Request)
EVENT_IMPL(Tox_Event_Friend_Status)
EVENT_IMPL(Tox_Event_Friend_Status_Message)
EVENT_IMPL(Tox_Event_Friend_Typing)

EVENT_IMPL(Tox_Event_Group_Peer_Name)
EVENT_IMPL(Tox_Event_Group_Peer_Status)
EVENT_IMPL(Tox_Event_Group_Topic)
EVENT_IMPL(Tox_Event_Group_Privacy_State)
EVENT_IMPL(Tox_Event_Group_Voice_State)
EVENT_IMPL(Tox_Event_Group_Topic_Lock)
EVENT_IMPL(Tox_Event_Group_Peer_Limit)
EVENT_IMPL(Tox_Event_Group_Password)
EVENT_IMPL(Tox_Event_Group_Message)
EVENT_IMPL(Tox_Event_Group_Private_Message)
EVENT_IMPL(Tox_Event_Group_Custom_Packet)
EVENT_IMPL(Tox_Event_Group_Custom_Private_Packet)
EVENT_IMPL(Tox_Event_Group_Invite)
EVENT_IMPL(Tox_Event_Group_Peer_Join)
EVENT_IMPL(Tox_Event_Group_Peer_Exit)
EVENT_IMPL(Tox_Event_Group_Self_Join)
EVENT_IMPL(Tox_Event_Group_Join_Fail)
EVENT_IMPL(Tox_Event_Group_Moderation)

