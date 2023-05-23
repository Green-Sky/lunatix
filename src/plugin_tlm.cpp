#include <solanaceae/plugin/solana_plugin_v1.h>

#include "./tox_lua_module.hpp"

#include <memory>
#include <iostream>

#define RESOLVE_INSTANCE(x) static_cast<x*>(solana_api->resolveInstance(#x))
#define PROVIDE_INSTANCE(x, p, v) solana_api->provideInstance(#x, p, static_cast<x*>(v))

static std::unique_ptr<ToxLuaModule> g_tlm = nullptr;

extern "C" {

SOLANA_PLUGIN_EXPORT const char* solana_plugin_get_name(void) {
	return "ToxLuaModule";
}

SOLANA_PLUGIN_EXPORT uint32_t solana_plugin_get_version(void) {
	return SOLANA_PLUGIN_VERSION;
}

SOLANA_PLUGIN_EXPORT uint32_t solana_plugin_start(struct SolanaAPI* solana_api) {
	//std::cout << "PLUGIN TLM START()\n";

	if (solana_api == nullptr) {
		return 1;
	}

	ToxI* tox_i = nullptr;
	ToxEventProviderI* tox_event_provider_i = nullptr;

	{ // make sure required types are loaded
		tox_i = RESOLVE_INSTANCE(ToxI);
		tox_event_provider_i = RESOLVE_INSTANCE(ToxEventProviderI);

		if (tox_i == nullptr) {
			std::cerr << "PLUGIN TLM missing ToxI\n";
			return 2;
		}

		if (tox_event_provider_i == nullptr) {
			std::cerr << "PLUGIN TLM missing ToxEventProviderI\n";
			return 2;
		}
	}

	// static store, could be anywhere tho
	// construct with fetched dependencies
	g_tlm = std::make_unique<ToxLuaModule>(*tox_i, *tox_event_provider_i);

	// register types
	PROVIDE_INSTANCE(ToxLuaModule, "ToxLuaModule", g_tlm.get());

	return 0;
}

SOLANA_PLUGIN_EXPORT void solana_plugin_stop(void) {
	//std::cout << "PLUGIN TLM STOP()\n";

	g_tlm.reset();
}

SOLANA_PLUGIN_EXPORT void solana_plugin_tick(float delta) {
	(void)delta;
	//std::cout << "PLUGIN TLM TICK()\n";

	g_tlm->iterate();
}

} // extern C

