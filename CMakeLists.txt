cmake_minimum_required(VERSION 3.20)
project(luso)

set(CMAKE_CXX_STANDARD 20)

add_library(lusoscript
	src/arena.cc
	src/ast.cc
	src/driver.cc
	src/environment.cc
	src/error.cc
	src/helper.cc
	src/interpreter.cc
	src/lexer.cc
	src/parser.cc
	src/repl.cc
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
