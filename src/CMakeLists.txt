cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

add_subdirectory(./solanaceae)

add_executable(lunatix
	./main.cpp
	./tox_lua_module.hpp
	./tox_lua_module.cpp
)

target_link_libraries(lunatix PUBLIC
	Luau.VM
	Luau.Compiler
	luabridge
	solanaceae_core
	solanaceae
)

target_compile_features(lunatix PUBLIC cxx_std_17)

