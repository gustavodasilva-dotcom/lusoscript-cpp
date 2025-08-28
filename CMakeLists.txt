cmake_minimum_required(VERSION 3.20)
project(luso)

set(CMAKE_CXX_STANDARD 20)

add_library(lusoscript
	src/error.cc
	src/interpreter.cc
	src/prompt.cc
	src/scanner.cc
	src/source_file.cc
	src/token.cc
)

target_include_directories(lusoscript
	PUBLIC
		${PROJECT_SOURCE_DIR}/include
)

add_executable(luso
	src/main.cc
)

target_link_libraries(luso
	PRIVATE lusoscript
)
