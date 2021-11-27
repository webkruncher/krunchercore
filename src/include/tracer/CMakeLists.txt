

file(GLOB includes ${CMAKE_CURRENT_LIST_DIR}/*.h)

install(
    FILES
	    ${includes}    
		    OPTIONAL
		    DESTINATION include/infokruncher/krunchercore/tracer
)

add_library(
    tracerheaders
	    INTERFACE
)

add_library( kruncher::tracerheaders ALIAS tracerheaders )

target_include_directories(
    tracerheaders
	    INTERFACE
		    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
		    $<INSTALL_INTERFACE:include>
)

target_link_libraries(
    tracerheaders
	    INTERFACE
)





