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
	solanaceae_toxcore
	solanaceae
)

target_compile_features(lunatix PUBLIC cxx_std_17)

#################################################

add_library(plugin_tlm SHARED
	./plugin_tlm.cpp

	./tox_lua_module.hpp
	./tox_lua_module.cpp
)

target_compile_features(plugin_tlm PUBLIC cxx_std_17)

target_link_libraries(plugin_tlm PUBLIC
	Luau.VM
	Luau.Compiler
	luabridge
	solanaceae_toxcore
	solanaceae_plugin
)

set_target_properties(plugin_tlm PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(plugin_tlm PROPERTIES C_VISIBILITY_PRESET hidden)

