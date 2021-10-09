cmake_minimum_required(VERSION 3.10)

project("webkruncher")

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CMakeLists.txt)

set(INCLUDE_PATHS)
IncludeProjects(
	libdb/include
	core/include
)
add_subdirectory( global )
add_subdirectory( ut )

enable_testing()
add_test(NAME krunchercoreut  COMMAND ../src.build/ui/krunchercoreut  )



