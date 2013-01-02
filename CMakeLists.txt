project(libism)
cmake_minimum_required(VERSION 2.4)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/" "${CMAKE_SOURCE_DIR}/soci/src/cmake/modules/")

find_package(Boost COMPONENTS program_options REQUIRED )
include_directories( ${Boost_INCLUDE_DIR} )

message(${CMAKE_VERSION})

# validation
add_definitions(-Wall -Wextra -pedantic -std=gnu++0x)

# GCC only sends a warning and not an error if it can't compile with stack canaries.
set(CMAKE_REQUIRED_FLAGS "-Werror")

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
add_subdirectory(soci/src)

include_directories(soci/src soci/src/backends/sqlite3)
add_executable(demorecorder demorecorder.cpp)
target_link_libraries(demorecorder ${LIBS} ${Boost_LIBRARIES} soci_core soci_sqlite3)
#add_executable(trainer trainer.cpp)
#add_executable(recognizer recognizer.cpp)
