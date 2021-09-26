

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
	set(INCPATH ~/Info)
else()
	set(INCPATH /usr/local/include/infokruncher)
endif()


function(IncludeProjects)
	foreach(arg IN LISTS ARGN)
		set(INCLUDE_PATHS ${INCLUDE_PATHS} " " ${INCPATH}/${arg})
	endforeach()
	set(INCLUDE_PATHS ${INCLUDE_PATHS} PARENT_SCOPE )
endfunction()

set(
	NIX_LIBRARY_PATHS
		/usr/local/lib/infokruncher/
)

set(
	NIX_LIBRARIES
			informationkruncher
			auth
			records
			site
			ssl	
			crypto	
)

IF(CMAKE_SYSTEM_NAME STREQUAL OpenBSD)
	add_definitions( -DUNIX )
	set(
		INCLUDE_PATHS
			${NIX_INCLUDE_PATHS}
	)
	set(
		LIBRARY_PATHS
			${NIX_LIBRARY_PATHS}
	)
	set(
		LIBRARIES
			${NIX_LIBRARIES}
	)
ENDIF()

IF(CMAKE_SYSTEM_NAME STREQUAL Linux)
	add_definitions( -DLINUX )
	set(
		INCLUDE_PATHS
			${NIX_INCLUDE_PATHS}
	)
	set(
		LIBRARY_PATHS
			${NIX_LIBRARY_PATHS}
			/usr/lib/x86_64-linux-gnu/
	)
	set(
		LIBRARIES
			${NIX_LIBRARIES}
			uuid
	)
ENDIF()





