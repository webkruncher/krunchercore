cmake_minimum_required(VERSION 3.10)

project("webkruncher")

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CMakeLists.txt)

set(INCLUDE_PATHS)
IncludeProjects(
	exexml/include
	libdb/include
	core/include
	informationkruncher/include
	informationkruncher/include/db
)
message( "PATHS: " ${INCLUDE_PATHS} )


add_subdirectory( global )

