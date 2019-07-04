cmake_minimum_required( VERSION 3.0 )
project ( SharpTNI VERSION 1.0 )

set( CMAKE_CXX_STANDARD 11 )
set( CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH} )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs" )
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release Profile" FORCE)
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" )
endif()

set( Boost_USE_STATIC_LIBS ON )

find_library( MATH_LIBRARIES m REQUIRED )
find_package( Threads REQUIRED )
find_package( LEMON REQUIRED )
find_package( Boost REQUIRED COMPONENTS thread system filesystem )

set( CommonLibs
  ${MATH_LIBRARIES}
  ${CMAKE_THREAD_LIBS_INIT}
  ${LEMON_LIBRARIES}
  ${Boost_LIBRARIES}
)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories( ${LEMON_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS} )

add_subdirectory(src)
