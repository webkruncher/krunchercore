
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
	set(INCMIDFIX src/include/)
	set(LIBMIDFIX src.build/)
	message("Using KRUNCHER_DEV_LIBS")
else()
	set(INCPATH /usr/local/include/infokruncher)
	set(LIBPATH /usr/local/lib/infokruncher)
	set(INCMIDFIX) 
	set(LIBMIDFIX)
	message("NOT Using KRUNCHER_DEV_LIBS")
endif()



function(Headers)
	set(INCLUDE_PATHS PARENT_SCOPE )
	foreach(arg IN LISTS ARGN)
		string(REPLACE "::" ";" PARTS ${arg})
		list (LENGTH PARTS llen)
		list(GET PARTS 0 A)

		IF(${llen} EQUAL 2)
			list(GET PARTS 1 B)
			string(CONCAT P ${INCPATH}/${A}/${INCMIDFIX}${B} " ")
			list(APPEND INCLUDE_PATHS ${P} )
		ENDIF(${llen} EQUAL 2)
		
		IF(${llen} EQUAL 1)
			list(APPEND INCLUDE_PATHS ${INCPATH}/${A}/${INCMIDFIX} " ")
		ENDIF(${llen} EQUAL 1)
	endforeach()	
	set(INCLUDE_PATHS ${INCLUDE_PATHS} PARENT_SCOPE )
	message( ${GreenColor} "INC:" ${INCLUDE_PATHS} ${NormalColor} )
endfunction()

function(LibraryPaths)
	set(LIBRARY_PATHS PARENT_SCOPE )
	foreach(arg IN LISTS ARGN)
		string(REPLACE "::" ";" PARTS ${arg})
		list (LENGTH PARTS llen)
		list(GET PARTS 0 A)
		list(GET PARTS 1 B)
		string(CONCAT P ${LIBPATH}/${A}/${LIBMIDFIX}${B} " ")
		list(APPEND LIBRARY_PATHS ${P} " " )
	endforeach()	
	set(LIBRARY_PATHS ${LIBRARY_PATHS} PARENT_SCOPE )
	message( ${YellowColor} "LibraryPaths:" ${LIBRARY_PATHS} ${NormalColor} )
endfunction()

function(Libraries)
	set(LIBRARIES PARENT_SCOPE )
	foreach(arg IN LISTS ARGN)
		list(APPEND LIBRARIES ${arg} " ")
	endforeach()	
	set(LIBRARIES ${LIBRARIES} PARENT_SCOPE )
	message( ${TealColor} "Libraries:" ${LIBRARIES} ${NormalColor} )
endfunction()

