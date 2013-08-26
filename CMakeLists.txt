project(ISM)
cmake_minimum_required(VERSION 2.4)

# validation
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=gnu++0x -O2")

# GCC only sends a warning and not an error if it can't compile with stack canaries.
set(CMAKE_REQUIRED_FLAGS "-Werror")

if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

find_package(Boost COMPONENTS system filesystem REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(ISM ./build)
GET_DIRECTORY_PROPERTY(ISM_INCLUDE_DIR DIRECTORY ISM DEFINITION ISM_INCLUDE_DIR)
GET_DIRECTORY_PROPERTY(ISM_LIBRARY_DIR DIRECTORY ISM DEFINITION ISM_LIBRARY_DIR)
set(ISM_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR} ${ISM_INCLUDE_DIR})
include_directories(${ISM_INCLUDE_DIR})
link_directories(${ISM_LIBRARY_DIR})

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ISM_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ISM_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ISM_BINARY_DIR}/bin)

add_executable(demorecorder src/demorecorder.cpp)
target_link_libraries(demorecorder ism)
add_executable(trainer src/trainer.cpp)
target_link_libraries(trainer ism)
add_executable(voteExtractor src/voteExtractor.cpp)
target_link_libraries(voteExtractor ism)
add_executable(demorecognizer src/demorecognizer.cpp)
target_link_libraries(demorecognizer ism)
add_executable(validator src/validator.cpp)
target_link_libraries(validator ism)
add_executable(dataMerger src/dataMerger.cpp)
target_link_libraries(dataMerger ism)
add_executable(modelCleaner src/modelCleaner.cpp)
target_link_libraries(modelCleaner ism)
add_executable(testRunner src/testRunner.cpp)
target_link_libraries(testRunner ism ${Boost_LIBRARIES})
