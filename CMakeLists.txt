# cmake_minimum_required(VERSION <specify CMake version here>)
project(nmap C)

set(CMAKE_C_STANDARD 11)

include_directories(incl)

add_executable(nmap
		incl/nmap.h
		src/main.c
		src/parser.c)
