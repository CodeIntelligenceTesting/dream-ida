#-------------------------------------------------------------------------------
# Copyright (C) 2011-2017 Khaled Yakdan.
# All rights reserved.
#-------------------------------------------------------------------------------
cmake_minimum_required( VERSION 3.0 FATAL_ERROR )

## Use the variable PROJECT_NAME for changing the target name
set( PROJECT_NAME "dream" )

## Set our project name
project( ${PROJECT_NAME} )
set( CMAKE_BUILD_TYPE Release )

## IDA files
set( IDA_ROOT "/home/user/ida-6.95" )

## find IDA library
find_library( IDA_LIB ida PATHS ${IDA_ROOT} )
if(NOT IDA_LIB)
	message( FATAL_ERROR "ida library was not found " )
endif()

## Directories for header files
include_directories( "${IDA_ROOT}/sdk/include" )
# This is a temporary solution since IDA's sdk 6.95 has a private include in intel.htpp (ins/pc.hpp)
# include_directories( "/home/user/Downloads/idasdk69/include" )


## IDA couldn't locate the shared objects of the Boost library
set(BOOST_LIBRARYDIR "/usr/lib/i386-linux-gnu")
set(BOOST_USE_STATIC_LIBS   ON)
find_package(Boost REQUIRED COMPONENTS system filesystem graph)

include_directories( ${Boost_INCLUDE_DIR} )

include_directories( ${PROJECT_SOURCE_DIR} )

## Use all the *.cpp files we found under this folder for the project
FILE( GLOB_RECURSE SOURCES "*.cpp" )

## Generate the shared library from the sources
add_library( ${PROJECT_NAME} SHARED ${SOURCES} )

target_compile_definitions( ${PROJECT_NAME} PUBLIC "-D__IDP__" "-D__PLUGIN__" "-D__LINUX__")
target_compile_options( ${PROJECT_NAME} PUBLIC "-m32" )


# SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -fpermissive")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")


set_target_properties( ${PROJECT_NAME} PROPERTIES 
  LINK_FLAGS "-m32 -Wl,--version-script=${IDA_ROOT}/sdk/plugins/plugin.script"
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED YES
  PREFIX ""
 	SUFFIX ".plx"
)

target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${IDA_LIB})

## Install the plugin in IDA's plugins directory
install(TARGETS ${PROJECT_NAME} DESTINATION "${IDA_ROOT}/plugins")
