
Banner()
get_filename_component(INCLUDES_PATH ../include/ REALPATH) 

add_library(
	krunchercoredirectorytest
		STATIC
			directorytest.cpp
)

add_library( kruncher::krunchercoredirectorytest ALIAS krunchercoredirectorytest  )

target_include_directories(
    krunchercoredirectorytest
	    PRIVATE
		../include/
		../../include/
		${INCLUDE_PATHS}
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_directories(
	krunchercoredirectorytest
		PUBLIC
			${LIBRARY_PATHS}
)

target_link_libraries(
    krunchercoredirectorytest
	    PUBLIC
		${LIBRARIES}
		ssl
		crypto
)

