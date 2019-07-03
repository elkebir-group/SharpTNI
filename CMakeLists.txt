cmake_minimum_required( VERSION 3.0 )

project ( SharpTNI VERSION 1.0 )

set( CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR} ${CMAKE_MODULE_PATH} )

set( sankoff_hdr
  src/utils.h
  src/basetree.h
  src/sankoff.h
  )

set( sankoff_src
  src/sankoffmain.cpp
  src/utils.cpp
  src/basetree.cpp
  src/sankoff.cpp
  )

set( sample_sankoff_hdr
  src/utils.h
  src/basetree.h
  src/sankoff.h
  )

set( sample_sankoff_src
  src/samplesankoffmain.cpp
  src/utils.cpp
  src/basetree.cpp
  src/sankoff.cpp
  )

set( dimacs_hdr
  src/utils.h
  src/basetree.h
  src/dimacs.h
  src/sankoff.h
  )

set( dimacs_src
  src/dimacsmain.cpp
  src/utils.cpp
  src/basetree.cpp
  src/sankoff.cpp
  src/dimacs.cpp
  )

set( gamma_hdr
  src/utils.h
  src/basetree.h
  )

set( gamma_src
  src/utils.cpp
  src/basetree.cpp
  src/gammamain.cpp
  )

set( LIBLEMON_ROOT "$ENV{HOME}/lemon" CACHE PATH "Additional search directory for lemon library" )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

if( ${CMAKE_SYSTEM_NAME} MATCHES "Darwin" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-local-typedefs" )
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: Debug Release Profile" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

set( Boost_USE_STATIC_LIBS ON )
find_package( Boost REQUIRED COMPONENTS thread system filesystem )

include_directories( "${LIBLEMON_ROOT}/include" "src" ${Boost_INCLUDE_DIRS} )
if (CPLEX)
  link_directories( ${CPLEX_LIB_DIR} ${CONCERT_LIB_DIR} "${LIBLEMON_ROOT}/lib" )
else()
  link_directories( "${LIBLEMON_ROOT}/lib" )
endif()

set( CommonLibs
  m
  emon
  pthread
  ${Boost_LIBRARIES}
)

set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG" )

add_executable( sankoff ${sankoff_src} ${sankoff_hdr} )
target_include_directories( sankoff PUBLIC "${LIBLEMON_ROOT}/include" "src" ${Boost_INCLUDE_DIRS})
target_link_libraries( sankoff ${CommonLibs} )

add_executable( sample_sankoff ${sample_sankoff_src} ${sample_sankoff_hdr} )
target_include_directories( sample_sankoff PUBLIC "${LIBLEMON_ROOT}/include" "src" ${Boost_INCLUDE_DIRS})
target_link_libraries( sample_sankoff ${CommonLibs} )

add_executable( dimacs ${dimacs_src} ${dimacs_hdr} )
target_include_directories( dimacs PUBLIC "${LIBLEMON_ROOT}/include" "src" ${Boost_INCLUDE_DIRS})
target_link_libraries( dimacs ${CommonLibs} )

add_executable( gamma ${gamma_src} ${gamma_hdr} )
target_include_directories( gamma PUBLIC "${LIBLEMON_ROOT}/include" "src" ${Boost_INCLUDE_DIRS})
target_link_libraries( gamma ${CommonLibs} )
