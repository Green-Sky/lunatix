#include <tox/tox.h>

#include "./tox_client.hpp"

#include <string_view>
#include <iostream>
#include <thread>
#include <cassert>
#include <cstdlib>

// https://youtu.be/YIE4b8gT3ho

int main(void) {
	ToxClient tcl;

	std::cout << "tox id: " << tcl.getOwnAddress() << "\n";

	while (tcl.iterate()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	return 0;
}

