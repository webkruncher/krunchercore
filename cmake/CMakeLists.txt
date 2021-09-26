



include(${CMAKE_CURRENT_LIST_DIR}/CMakeUtilities.txt)


set(CMAKE_CXX_FLAGS " -pthread " )
add_compile_options(-ggdb)
set(CMAKE_CXX_COMPILER "g++")


IF(CMAKE_SYSTEM_NAME STREQUAL OpenBSD)
	add_definitions( -DUNIX -Wall )
ENDIF()

IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
	add_definitions( -DLINUX -Wall )
ENDIF()


if(DEFINED ENV{KRUNCHER_DEV_LIBS})
	set(INCPATH /home/jmt/Info)
	set(LIBPATH /home/jmt/Info)
else()
	set(INCPATH /usr/local/include/infokruncher)
	set(LIBPATH /usr/local/lib/infokruncher)
endif()


function(IncludeProjects)
	foreach(arg IN LISTS ARGN)
		set(INCLUDE_PATHS ${INCLUDE_PATHS} ${INCPATH}/${arg})
	endforeach()
	set(INCLUDE_PATHS ${INCLUDE_PATHS} PARENT_SCOPE )
	message(${BlueBkColor}${YellowColor}${CMAKE_CURRENT_LIST_DIR}${NormalColor}${GreenColor}${INCLUDE_PATHS}${NormalColor} )
endfunction()

function(IncludeLibraryPaths)
	foreach(arg IN LISTS ARGN)
		set(LIBRARY_PATHS ${LIBRARY_PATHS} ${LIBPATH}/${arg})
	endforeach()
	set(LIBRARY_PATHS ${LIBRARY_PATHS} PARENT_SCOPE )
	message(${TealBkColor}${BlackColor}${CMAKE_CURRENT_LIST_DIR}${NormalColor}${GreenColor}${LIBRARY_PATHS}${NormalColor} )
endfunction()

function(IncludeLibraries)
	foreach(arg IN LISTS ARGN)
		set(LIBRARIES ${LIBRARIES} ${arg})
	endforeach()
	set(LIBRARY_PATHS ${LIBRARY_PATHS} PARENT_SCOPE )
	message(${YellowBkColor}${BlackColor}${CMAKE_CURRENT_LIST_DIR}${NormalColor}${GreenColor}${LIBRARIES}${NormalColor} )
endfunction()

