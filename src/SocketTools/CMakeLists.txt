cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)
project(SocketTools)

set(CMAKE_CXX_STANDARD 11)

file(GLOB srcs "*.cpp" "*.h" "*.hpp")
file(GLOB includes
	"${CMAKE_SOURCE_DIR}/includes/${PROJECT_NAME}/*.h"
	"${CMAKE_SOURCE_DIR}/includes/${PROJECT_NAME}/*.hpp")
add_library(${PROJECT_NAME} STATIC ${srcs} ${includes})

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_SOURCE_DIR}/includes/${PROJECT_NAME}/)

set_target_properties(${PROJECT_NAME} PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/bin"
	LIBRARY_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/bin"
	RUNTIME_OUTPUT_DIRECTORY	"${CMAKE_SOURCE_DIR}/bin"
	EXECUTABLE_OUTPUT_PATH		"${CMAKE_SOURCE_DIR}/bin"
)

# Flags
String(CONCAT Flags ${Flags} " " "-Wall")
String(CONCAT Flags ${Flags} " " "-Werror")
String(CONCAT Flags ${Flags} " " "-Waddress")
String(CONCAT Flags ${Flags} " " "-Waggregate-return")
Set(CMAKE_CXX_FLAGS "${Flags}")