cmake_minimum_required(VERSION 3.21)

project(WILSON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB_RECURSE WILSON_SRC src/*.*)

add_executable(WILSON
	${WILSON_SRC})
