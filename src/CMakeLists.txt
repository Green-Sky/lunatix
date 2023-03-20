cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

add_executable(tmp
	./tmp.cpp
)

target_link_libraries(tmp PUBLIC toxcore)

