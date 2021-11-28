
add_subdirectory( Directory )

get_filename_component(INCLUDES_PATH ../include/ REALPATH) 

add_library(
	krunchercoreutlib
		STATIC
			mimetester.cpp
)

add_library( kruncher::krunchercoreutlib ALIAS krunchercoreutlib  )

target_include_directories(
    krunchercoreutlib
	    PRIVATE
		include/
		./Directory/
		../include/
		../include/tracer
		${INCLUDE_PATHS}
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_directories(
	krunchercoreutlib
		PUBLIC
			${LIBRARY_PATHS}
)

target_link_libraries(
    krunchercoreutlib
	    PUBLIC
		${LIBRARIES}
		ssl
		crypto
		krunchercoredirectorytest
)

	    
install(TARGETS krunchercoreutlib DESTINATION lib/infokruncher )

add_executable(
	krunchercoreut
		ut.cpp
)


target_include_directories(
    krunchercoreut
	    PRIVATE
		./include/
		../include/
		./Directory/
		${INCLUDE_PATHS}
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_directories(
	krunchercoreut
		PUBLIC
			${LIBRARY_PATHS}
)

target_link_libraries(
    krunchercoreut
	    PUBLIC
		${LIBRARIES}
		ssl
		crypto
		krunchercore
		krunchercoredirectorytest
		krunchercoreutlib
)

