add_executable( appbase_example main.cpp yarp_plugin.h)
target_link_libraries( appbase_example appbase ${CMAKE_DL_LIBS} ${PLATFORM_SPECIFIC_LIBS} )
