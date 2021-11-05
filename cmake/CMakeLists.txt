
include(${CMAKE_CURRENT_LIST_DIR}/CMakeUtilities.txt)


set(CMAKE_CXX_FLAGS " -pthread " )
add_compile_options(-ggdb  -gdwarf-2  )
#set(CMAKE_CXX_COMPILER "g++")
#set(CMAKE_CXX_COMPILER "clang++")


IF(CMAKE_SYSTEM_NAME STREQUAL OpenBSD)
	#add_definitions( -DUNIX -Wall )
	#add_definitions( -DUNIX )

	set(CMAKE_CXX_STANDARD 11)
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
	set(CMAKE_CXX_EXTENSIONS OFF)

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
	#message(${BlueBkColor}${YellowColor}${CMAKE_CURRENT_LIST_DIR}${NormalColor}${GreenColor}${INCLUDE_PATHS}${NormalColor} )
endfunction()

function(IncludeLibraryPaths)
	foreach(arg IN LISTS ARGN)
		execute_process(
			COMMAND bash "-c" "find ${LIBPATH}/${arg}/src.build -name \"*.a\" -exec dirname {} \; | xargs | tr ' ' ';' "
			OUTPUT_VARIABLE TMPLIBPATHS
		)
		execute_process(
			COMMAND bash "-c" "find ${LIBPATH}/${arg}/src.build -name \"*.a\" -exec basename {} \; | sed 's/^lib//g' | sed 's/\.a$//g'  | xargs | tr ' ' ';' "
			OUTPUT_VARIABLE TMPLIBS
		)
		#message("TMPLIBS:${TMPLIBS}")
		string(STRIP "${TMPLIBS}" TMPLIBS )
		set(LIBRARIES ${LIBRARIES};${TMPLIBS})
		#message("TMPLIBPATHS:${TMPLIBPATHS}")
		string(STRIP "${TMPLIBPATHS}" TMPLIBPATHS )
		set(LIBRARY_PATHS ${LIBRARY_PATHS};${TMPLIBPATHS})
	endforeach()
	set(LIBRARY_PATHS ${LIBRARY_PATHS} PARENT_SCOPE )
	set(LIBRARIES ${LIBRARIES} PARENT_SCOPE )
	#message(LIBRPATHS:${TealBkColor}${BlackColor}${CMAKE_CURRENT_LIST_DIR}${NormalColor}${GreenColor}${LIBRARY_PATHS}${NormalColor} )
	#message(LIBRARIES:${BlueBkColor}${GreenColor}${CMAKE_CURRENT_LIST_DIR}${NormalColor}${GreenColor}${LIBRARIES}${NormalColor} )
endfunction()

function(IncludeLibraries)
	foreach(arg IN LISTS ARGN)
		set(LIBRARIES ${LIBRARIES} ${arg})
	endforeach()
	set(LIBRARY_PATHS ${LIBRARY_PATHS} PARENT_SCOPE )
	#message(${YellowBkColor}${BlackColor}${CMAKE_CURRENT_LIST_DIR}${NormalColor}${GreenColor}${LIBRARIES}${NormalColor} )
endfunction()

