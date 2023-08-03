#include <tox/tox.h>

#include <solanaceae/toxcore/tox_event_logger.hpp>
#include "./solanaceae/tox_client.hpp"
#include "./solanaceae/auto_dirty.hpp"
#include "./solanaceae/transfer_manager.hpp"

#include "./tox_lua_module.hpp"

#include <string_view>
#include <iostream>
#include <thread>
#include <cassert>
#include <cstdlib>

// https://youtu.be/YIE4b8gT3ho

int main(void) {
	std::cout << "LUNATiX - because we have to be insane\n";

	ToxEventLogger tel{std::cout};
	ToxClient tc{"lunatix.tox"};
	tc.setSelfName("LUNATiX"); // TODO: this is ugly

	tel.subscribeAll(tc);
	AutoDirty ad{tc};

	TransferManager tm{tc, tc};

	ToxLuaModule tlm{tc, tc};

	std::cout << "tox id: " << tc.toxSelfGetAddressStr() << "\n";

	while (tc.iterate()) {
		tm.iterate(); // currently does nothing
		tlm.iterate();
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	return 0;
}

