#-------------------------------
# PROJECT INFORMATION
#-------------------------------

cmake_minimum_required(VERSION 3.16.3)
project(frameworkd VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs) # Installation directories for `install` command and pkgconfig file

#-------------------------------
# PROJECT MACROS DEFINITION
#-------------------------------

macro(ZFKD_ADD_DEPS_FILEPATHS_TO outputList baseDir subDir depsList fileExt)
    
    foreach(dependency IN LISTS ${depsList})
        list(APPEND ${outputList} ${baseDir}/${subDir}/${dependency}/${dependency}.${fileExt})
    endforeach()

endmacro(ZFKD_ADD_DEPS_FILEPATHS_TO)


macro(ZFKD_ADD_DEPS_DIRPATHS_TO outputList baseDir subDir depsList)
    
    foreach(dependency IN LISTS ${depsList})
        list(APPEND ${outputList} ${baseDir}/${subDir}/${dependency}/)
    endforeach()

endmacro(ZFKD_ADD_DEPS_DIRPATHS_TO)


macro(ZFKD_CREATE_DICT contentList indexList dictName)
    
    set(i 0)
    foreach(element IN LISTS ${contentList})
        list(GET ${indexList} ${i} index)
        set(${dictName}-${index} ${element})
        math(EXPR i "${i} + 1")
    endforeach()

endmacro(ZFKD_CREATE_DICT)


#------------------
# SET SOURCE FILES
#------------------

set(ZFKD_CLASSES
    
    daemon
    service-handler
    service-proxy
    static-service-proxy
    routine-service-proxy
)

set(ZFKD_UTILITIES

    dbus-handler
    config-handler
    locked-storage
)

set(ZFKD_PUBLIC_CLASSES
    daemon
)

set(ZFKD_PUBLIC_UTILITIES
	
)

#----------------------------
# SOURCE FILES CONFIGURATION
#----------------------------

set(ZFKD_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(ZFKD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(ZFKD_UTIL_SUBDIR utilities)
set(ZFKD_CLSS_SUBDIR classes)

set(ZFKD_CPP_SRCS)

ZFKD_ADD_DEPS_FILEPATHS_TO(ZFKD_CPP_SRCS ${ZFKD_SRC_DIR} ${ZFKD_CLSS_SUBDIR} ZFKD_CLASSES cpp)
ZFKD_ADD_DEPS_FILEPATHS_TO(ZFKD_CPP_SRCS ${ZFKD_SRC_DIR} ${ZFKD_UTIL_SUBDIR} ZFKD_UTILITIES cpp)

set(ZFKD_HDR_SRCS)

ZFKD_ADD_DEPS_FILEPATHS_TO(ZFKD_HDR_SRCS ${ZFKD_SRC_DIR} ${ZFKD_CLSS_SUBDIR} ZFKD_CLASSES hpp)
ZFKD_ADD_DEPS_FILEPATHS_TO(ZFKD_HDR_SRCS ${ZFKD_SRC_DIR} ${ZFKD_UTIL_SUBDIR} ZFKD_UTILITIES hpp)

set(ZFKD_PUBLIC_HDRS)

ZFKD_ADD_DEPS_FILEPATHS_TO(ZFKD_PUBLIC_HDRS ${ZFKD_INCLUDE_DIR} ${ZFKD_CLSS_SUBDIR} ZFKD_PUBLIC_CLASSES hpp)
ZFKD_ADD_DEPS_FILEPATHS_TO(ZFKD_PUBLIC_HDRS ${ZFKD_INCLUDE_DIR} ${ZFKD_UTIL_SUBDIR} ZFKD_PUBLIC_UTILITIES hpp)

set(ZFKD_SRCS ${ZFKD_CPP_SRCS} ${ZFKD_HDR_SRCS} ${ZFKD_PUBLIC_HDRS})

#-------------------------------
# GENERAL COMPILER CONFIGURATION
#-------------------------------

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CXX_FLAGS "-pthread ${CMAKE_CXX_FLAGS}")

#-----------------------------------
# INCLUDE CONAN SETINGS FOR DEVELOPERS
#-----------------------------------

option(ZFKD_USING_CONAN_FOR_DEV "(default ON)" ON)

if(ZFKD_USING_CONAN_FOR_DEV)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
endif()

#----------------------------------
# LIBRARY BUILD INFORMATION
#----------------------------------

set(ZFKD_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(ZFKD_VERSION "${PROJECT_VERSION}")

set(ZFKD_SRC_DIRS)
ZFKD_ADD_DEPS_DIRPATHS_TO(ZFKD_SRC_DIRS ${ZFKD_SRC_DIR} ${ZFKD_CLSS_SUBDIR} ZFKD_CLASSES)
ZFKD_ADD_DEPS_DIRPATHS_TO(ZFKD_SRC_DIRS ${ZFKD_SRC_DIR} ${ZFKD_UTIL_SUBDIR} ZFKD_UTILITIES)

ZFKD_CREATE_DICT(ZFKD_SRC_DIRS ZFKD_DEPS ZFKD_DIR)

add_library(frameworkd-objlib OBJECT ${ZFKD_SRCS})
target_compile_definitions(frameworkd-objlib PRIVATE BUILD_LIB=1)

target_include_directories(frameworkd-objlib PUBLIC    
	
	$<BUILD_INTERFACE:${ZFKD_DIR-daemon}>
	$<BUILD_INTERFACE:${ZFKD_DIR-service-handler}>
	$<BUILD_INTERFACE:${ZFKD_DIR-service-proxy}>
	$<BUILD_INTERFACE:${ZFKD_DIR-service}>
	$<BUILD_INTERFACE:${ZFKD_DIR-dbus-handler}>
	$<BUILD_INTERFACE:${ZFKD_DIR-config-handler}>
	$<BUILD_INTERFACE:${ZFKD_DIR-locked-storage}>
)

if(DEFINED BUILD_SHARED_LIBS)
    set_target_properties(frameworkd-objlib PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
endif()

add_library(frameworkd)
target_include_directories(frameworkd PUBLIC 
	
	$<BUILD_INTERFACE:${ZFKD_DIR-daemon}>
	$<BUILD_INTERFACE:${ZFKD_DIR-service-handler}>
	$<BUILD_INTERFACE:${ZFKD_DIR-service-proxy}>
	$<BUILD_INTERFACE:${ZFKD_DIR-service}>
	$<BUILD_INTERFACE:${ZFKD_DIR-dbus-handler}>
	$<BUILD_INTERFACE:${ZFKD_DIR-config-handler}>
	$<BUILD_INTERFACE:${ZFKD_DIR-locked-storage}>
	$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(frameworkd
	
	PROPERTIES PUBLIC_HEADER "${ZFKD_PUBLIC_HDRS}"
	VERSION "${ZFKD_VERSION}"
	OUTPUT_NAME "frameworkd"
)

if(ZFKD_USING_CONAN_FOR_DEV)
	
	target_link_libraries(frameworkd PRIVATE frameworkd-objlib ${CONANLIBS})

else()
    
	target_link_libraries(frameworkd PRIVATE frameworkd-objlib)

endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#----------------------------------
# INSTALLATION
#----------------------------------

# COMMING SOON...


#----------------------------------
# TESTS
#----------------------------------

option(BUILD_TESTS "Build and install tests (default OFF)" OFF)

if(BUILD_TESTS)
	message(STATUS "Building with tests")
	enable_testing()
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
endif()


#------
# BIN
#------
# Uncoment this section to build your binnaries with this CMake
# NOTE: This section is temporary!!
#        It will persist until the maintainer solve problems with outside inclusion by "include_subdirectory" mode.
#
#
# add_executable(sample ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
# target_link_libraries(sample ${CONAN_LIBS} frameworkd)


