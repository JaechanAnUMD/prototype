# GoogleTest
include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/e4717df71a4f45bf9f0ac88c6cd9846a0bc248dd.zip)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

set(NETWORK_TESTS
    network_test.cc
)

add_executable(network_test ${NETWORK_TESTS})

target_link_libraries(
    network_test
    network
    gtest_main
)

include(GoogleTest)
gtest_discover_tests(network_test)

