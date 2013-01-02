project(libism)
cmake_minimum_required(VERSION 2.4)
message(${CMAKE_VERSION})

# validation
add_definitions(-Wall -Wextra -pedantic)

# GCC only sends a warning and not an error if it can't compile with stack canaries.
set(CMAKE_REQUIRED_FLAGS "-Werror")

add_executable(demorecorder demorecorder.cpp)
#add_executable(trainer trainer.cpp)
#add_executable(recognizer recognizer.cpp)
