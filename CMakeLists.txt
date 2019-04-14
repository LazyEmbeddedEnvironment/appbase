cmake_minimum_required(VERSION 3.5)
project(libappbase VERSION 1.0.0 LANGUAGES CXX)

##############################################
#Make sure that custom modules like FindRapidJSON are found
list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_SOURCE_DIR}/cmake)

##############################################
# Declare dependencies
find_package(Boost 1.69 REQUIRED COMPONENTS date_time
                                            filesystem
                                            system
                                            chrono
                                            program_options
                                            unit_test_framework)
find_package (Threads)

##############################################
# Create target and set properties

add_library(appbase
        application.cpp
        version.cpp)

#Add an alias so that library can be used inside the build tree, e.g. when testing
add_library(AppBase::appbase ALIAS appbase)

#Set target properties
target_include_directories(appbase
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_features(appbase PUBLIC cxx_std_17)
target_compile_options(appbase PUBLIC $<$<CXX_COMPILER_ID:GNU>:-Wall>)

target_link_libraries(appbase
        PUBLIC
        Boost::boost
        Boost::date_time
        Boost::filesystem
        Boost::system
        Boost::chrono
        Boost::program_options
        Boost::unit_test_framework
        ${CMAKE_THREAD_LIBS_INIT}
        )

# Version injector to source
find_package(Git)
if(EXISTS ${CMAKE_SOURCE_DIR}/.git AND GIT_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/version.cmake @ONLY)
    add_custom_target(appbase_version ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/version.cmake BYPRODUCTS version.cpp)
    add_dependencies(appbase appbase_version)
else()
    set(VERSION_STRING "Unknown")
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.cpp.in ${CMAKE_CURRENT_BINARY_DIR}/version.cpp @ONLY ESCAPE_QUOTES)
endif()

##############################################
# Installation instructions

include(GNUInstallDirs)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/AppBase)

install(TARGETS appbase
        EXPORT appbase-targets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        )

#This is required so that the exported target has the name AppBase and not appbase
set_target_properties(appbase PROPERTIES EXPORT_NAME AppBase)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#Export the targets to a script
install(EXPORT appbase-targets
        FILE
        AppBaseTargets.cmake
        NAMESPACE
        AppBase::
        DESTINATION
        ${INSTALL_CONFIGDIR}
        )

#Create a ConfigVersion.cmake file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/AppBaseConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/AppBaseConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/AppBaseConfig.cmake
        INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
        )

#Install the config, configversion and custom find modules
install(FILES
        ${CMAKE_CURRENT_LIST_DIR}/cmake/FindRapidJSON.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/AppBaseConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/AppBaseConfigVersion.cmake
        DESTINATION ${INSTALL_CONFIGDIR}
        )

##############################################

export(EXPORT appbase-targets FILE ${CMAKE_CURRENT_BINARY_DIR}/AppBaseTargets.cmake NAMESPACE AppBase::)

#Register package in user's package registry
export(PACKAGE AppBase)

add_subdirectory(examples)
