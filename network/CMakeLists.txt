cmake_minimum_required(VERSION 3.21)

project(network VERSION 1.0.0)

configure_file(NetworkConfig.h.in NetworkConfig.h)

include(CTest)

# C++ settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Options for libraries
option(USE_NETWORK "Use the Network library" ON)
option(USE_GOOGLE_TEST "Use GoogleTest for testing" ON)

# Enable or disable to change concurrency policy
#add_compile_definitions(BUCKET_LOCKING)
#add_compile_definitions(CHAIN_LOCKING)
add_compile_definitions(OPTIMISTIC_LOCKING)

# Network project library
if(USE_NETWORK)
    add_subdirectory(network)
    list(APPEND EXTRA_LIBS network)
endif()

# GoogleTest
if(USE_GOOGLE_TEST)
    add_subdirectory(test)
endif()

# Master program
set(MASTER_PROJECT_NAME master)
add_executable(${MASTER_PROJECT_NAME} master.cc)

target_link_libraries(${MASTER_PROJECT_NAME} PUBLIC ${EXTRA_LIBS})

target_include_directories(${MASTER_PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)

# Client program
set(CLIENT_PROJECT_NAME client)
add_executable(${CLIENT_PROJECT_NAME} client.cc)

target_link_libraries(${CLIENT_PROJECT_NAME} PUBLIC ${EXTRA_LIBS})

target_include_directories(${CLIENT_PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)

# Node program
set(NODE_PROJECT_NAME node)
add_executable(${NODE_PROJECT_NAME} node.cc)

target_link_libraries(${NODE_PROJECT_NAME} PUBLIC ${EXTRA_LIBS})

target_include_directories(${NODE_PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)

# Benchmark program
#set(BENCHMARK_PROJECT_NAME benchmark)
#add_executable(${BENCHMARK_PROJECT_NAME} benchmark.cc)
#
#target_link_libraries(${BENCHMARK_PROJECT_NAME} PUBLIC ${EXTRA_LIBS})
#
#target_include_directories(${BENCHMARK_PROJECT_NAME} PUBLIC
#    "${PROJECT_BINARY_DIR}"
#)

