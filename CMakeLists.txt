#-------------------------------
# PROJECT INFORMATION
#-------------------------------

cmake_minimum_required(VERSION 3.18)
project(frameworkd VERSION 0.1 LANGUAGES CXX)

include(GNUInstallDirs) # Installation directories for `install` command and pkgconfig file

#-------------------------------
# PROJECT MACROS DEFINITION
#-------------------------------

macro(FRAMEWORKD_ADD_DEPS_FILEPATHS_TO outputList baseDir subDir depsList fileExt)
	
	foreach(dependency IN LISTS ${depsList})
		list(APPEND ${outputList} ${baseDir}/${subDir}/${dependency}/${dependency}.${fileExt})
	endforeach()

endmacro(FRAMEWORKD_ADD_DEPS_FILEPATHS_TO)


macro(FRAMEWORKD_ADD_DEPS_DIRPATHS_TO outputList baseDir subDir depsList)
	
	foreach(dependency IN LISTS ${depsList})
		list(APPEND ${outputList} ${baseDir}/${subDir}/${dependency}/)
	endforeach()

endmacro(FRAMEWORKD_ADD_DEPS_DIRPATHS_TO)


macro(FRAMEWORKD_CREATE_DICT contentList indexList dictName)
	
	set(i 0)
	foreach(element IN LISTS ${contentList})
		list(GET ${indexList} ${i} index)
		set(${dictName}-${index} ${element})
		math(EXPR i "${i} + 1")
	endforeach()

endmacro(FRAMEWORKD_CREATE_DICT)


#------------------
# SET SOURCE FILES
#------------------

set(FRAMEWORKD_CLASSES
						daemon
						service
						service-handler)

set(FRAMEWORKD_UTILITIES
						dbus-handler)
#----------------------------
# SOURCE FILES CONFIGURATION
#----------------------------

set(FRAMEWORKD_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(FRAMEWORKD_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(FRAMEWORKD_UTIL_SUBDIR utilities)
set(FRAMEWORKD_CLSS_SUBDIR classes)

set(FRAMEWORKD_CPP_SRCS)

FRAMEWORKD_ADD_DEPS_FILEPATHS_TO(FRAMEWORKD_CPP_SRCS ${FRAMEWORKD_SOURCE_DIR} ${FRAMEWORKD_CLSS_SUBDIR} FRAMEWORKD_CLASSES cpp)
FRAMEWORKD_ADD_DEPS_FILEPATHS_TO(FRAMEWORKD_CPP_SRCS ${FRAMEWORKD_SOURCE_DIR} ${FRAMEWORKD_UTIL_SUBDIR} FRAMEWORKD_UTILITIES cpp)

set(FRAMEWORKD_HDR_SRCS)

FRAMEWORKD_ADD_DEPS_FILEPATHS_TO(FRAMEWORKD_HDR_SRCS ${FRAMEWORKD_SOURCE_DIR} ${FRAMEWORKD_CLSS_SUBDIR} FRAMEWORKD_CLASSES hpp)
FRAMEWORKD_ADD_DEPS_FILEPATHS_TO(FRAMEWORKD_HDR_SRCS ${FRAMEWORKD_SOURCE_DIR} ${FRAMEWORKD_UTIL_SUBDIR} FRAMEWORKD_UTILITIES hpp)

set(FRAMEWORKD_PUBLIC_HDRS)

FRAMEWORKD_ADD_DEPS_FILEPATHS_TO(FRAMEWORKD_PUBLIC_HDRS ${FRAMEWORKD_INCLUDE_DIR} ${FRAMEWORKD_CLSS_SUBDIR} FRAMEWORKD_CLASSES hpp)
FRAMEWORKD_ADD_DEPS_FILEPATHS_TO(FRAMEWORKD_PUBLIC_HDRS ${FRAMEWORKD_INCLUDE_DIR} ${FRAMEWORKD_UTIL_SUBDIR} FRAMEWORKD_UTILITIES hpp)

set(FRAMEWORKD_SRCS ${FRAMEWORKD_CPP_SRCS} ${FRAMEWORKD_HDR_SRCS} ${FRAMEWORKD_PUBLIC_HDRS})

add_library(troops OBJECT ${FRAMEWORKD_SRCS})

#-------------------------------
# GENERAL COMPILER CONFIGURATION
#-------------------------------

set(CMAKE_CXX_STANDARD 17)

#-----------------------------------
# INCLUDE CONAN SETINGS FOR DEVELOPERS
#-----------------------------------

option(FRAMEWORKD_USING_CONAN_FOR_DEV "(default ON)" ON)

if(FRAMEWORKD_USING_CONAN_FOR_DEV)
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
endif()

#----------------------------------
# LIBRARY BUILD INFORMATION
#----------------------------------

set(FRAMEWORKD_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(FRAMEWORKD_VERSION "${PROJECT_VERSION}")

set(FRAMEWORKD_SRC_DIRS)
FRAMEWORKD_ADD_DEPS_DIRPATHS_TO(FRAMEWORKD_SRC_DIRS ${FRAMEWORKD_SOURCE_DIR} ${FRAMEWORKD_CLSS_SUBDIR} FRAMEWORKD_CLASSES)
FRAMEWORKD_ADD_DEPS_DIRPATHS_TO(FRAMEWORKD_SRC_DIRS ${FRAMEWORKD_SOURCE_DIR} ${FRAMEWORKD_UTIL_SUBDIR} FRAMEWORKD_UTILITIES)

FRAMEWORKD_CREATE_DICT(FRAMEWORKD_SRC_DIRS FRAMEWORKD_DEPS FRAMEWORKD_DIR)

add_library(frameworkd-objlib OBJECT ${FRAMEWORKD_SRCS})
target_compile_definitions(frameworkd-objlib PRIVATE BUILD_LIB=1)

target_include_directories(frameworkd-objlib PUBLIC	$<BUILD_INTERFACE:${FRAMEWORKD_DIR-daemon}>
													$<BUILD_INTERFACE:${FRAMEWORKD_DIR-service}>
													$<BUILD_INTERFACE:${FRAMEWORKD_DIR-service-handler}>
													$<BUILD_INTERFACE:${FRAMEWORKD_DIR-dbus-handler}>
)

if(DEFINED BUILD_SHARED_LIBS)
    set_target_properties(frameworkd-objlib PROPERTIES POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
endif()

add_library(frameworkd)
target_include_directories(frameworkd PUBLIC 	$<BUILD_INTERFACE:${FRAMEWORKD_DIR-daemon}>
												$<BUILD_INTERFACE:${FRAMEWORKD_DIR-service}>
												$<BUILD_INTERFACE:${FRAMEWORKD_DIR-service-handler}>
												$<BUILD_INTERFACE:${FRAMEWORKD_DIR-dbus-handler}>
												$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>)

set_target_properties(frameworkd
								PROPERTIES PUBLIC_HEADER "${FRAMEWORKD_PUBLIC_HDRS}"
								VERSION "${FRAMEWORKD_VERSION}"
                                OUTPUT_NAME "frameworkd")

if(FRAMEWORKD_USING_CONAN_FOR_DEV)
	target_link_libraries(frameworkd PRIVATE frameworkd-objlib ${CONANLIBS})
else()
	target_link_libraries(frameworkd PRIVATE frameworkd-objlibs)
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
#		It will persist until the maintainer solve problems with outside inclusion by "include_subdirectory" mode.
#
#
#add_executable(sample ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
#target_link_libraries(sample ${CONAN_LIBS} frameworkd)


