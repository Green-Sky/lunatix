cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(solanaceae)

add_library(solanaceae
	./tox_client.hpp
	./tox_client.cpp
	./auto_dirty.hpp
	./auto_dirty.cpp

	./transfer_manager.hpp
	./transfer_manager.cpp
)

target_link_libraries(solanaceae PUBLIC
	solanaceae_core
)


