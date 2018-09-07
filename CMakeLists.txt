cmake_minimum_required(VERSION 3.6)
project(nmap)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
		incl/nmap.h
		src/main.c)

add_executable(nmap ${SOURCE_FILES})