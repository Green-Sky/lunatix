cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

add_executable(lunatix
	./tox_client.hpp
	./tox_client.cpp
	./tox_utils.hpp
	./tox_utils.cpp
	./main.cpp
)

target_link_libraries(lunatix PUBLIC
	toxcore
	Luau.VM
	Luau.Compiler
)

target_compile_features(lunatix PUBLIC cxx_std_17)

