

get_filename_component(INCLUDES_PATH ../include/ REALPATH) 

add_library(
	krunchercore
		STATIC
			globals.cpp
)

add_library( kruncher::krunchercore  ALIAS krunchercore  )

target_include_directories(
    krunchercore
	    PRIVATE
		../include/
		${INCLUDE_PATHS}
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_directories(
	krunchercore
		PUBLIC
			${LIBRARY_PATHS}
)

target_link_libraries(
    krunchercore
	    PUBLIC
		${LIBRARIES}
		ssl
		crypto
)

	    
install(TARGETS krunchercore DESTINATION lib/infokruncher )

