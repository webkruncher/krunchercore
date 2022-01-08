

Banner()

get_filename_component(INCLUDES_PATH ../../include/ REALPATH) 

add_library(
	krunchertrace
		STATIC
			tracer.cpp
)

add_library( kruncher::krunchertrace  ALIAS krunchertrace  )

target_include_directories(
    krunchertrace
	    PRIVATE
		../../include/
		../../include/tracer
		${INCLUDE_PATHS}
		$<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		$<INSTALL_INTERFACE:include>
)

target_link_directories(
	krunchertrace
		PUBLIC
			${LIBRARY_PATHS}
)

target_link_libraries(
    krunchertrace
	    PUBLIC
		${LIBRARIES}
)

	    
install(TARGETS krunchertrace DESTINATION lib/infokruncher/krunchercore/global/tracer )

