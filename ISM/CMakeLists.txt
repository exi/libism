project(LIBISM)
cmake_minimum_required(VERSION 2.4)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/soci/src/cmake/modules/")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules/")

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

set(WITH_MYSQL OFF)
set(SOCI_MYSQL OFF)
set(WITH_ODBC OFF)
set(SOCI_ODBC OFF)
set(WITH_ORACLE OFF)
set(SOCI_ORACLE OFF)
set(WITH_POSTGRESQL OFF)
set(SOCI_POSTGRESQL OFF)

add_subdirectory(soci/src ./build)

find_package(Boost COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

message(${CMAKE_VERSION})

# validation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=gnu++0x -g -O2")
# GCC only sends a warning and not an error if it can't compile with stack canaries.
set(CMAKE_REQUIRED_FLAGS "-Werror")

FILE(GLOB CPPFILES *.cpp)

set(ISM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/soci/src/core ${CMAKE_CURRENT_SOURCE_DIR}/soci/src/backends/sqlite3 ${EIGEN3_INCLUDE_DIR} ${BOOST_INCLUDE_DIR})
set(ISM_LIBRARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIBISM_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIBISM_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${LIBISM_BINARY_DIR}/bin)

include_directories(${ISM_INCLUDE_DIR})
add_library(ism SHARED ${CPPFILES})
target_link_libraries(ism ${LIBS} ${Boost_LIBRARIES} soci_core soci_sqlite3)
set_target_properties(ism PROPERTIES LINKER_LANGUAGE CXX)
