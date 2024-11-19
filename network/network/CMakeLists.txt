# Sources
set(NETWORK_SOURCE_DIR src)
set(NETWORK_SOURCES
    ${NETWORK_SOURCE_DIR}/network.cc
    ${NETWORK_SOURCE_DIR}/global.cc
)

# Headers
set(NETWORK_HEADER_DIR include)
set(NETWORK_HEADERS
    ${NETWORK_HEADER_DIR}/network.h
    ${NETWORK_HEADER_DIR}/merkle_tree.h
)

add_library(network STATIC ${NETWORK_HEADERS} ${NETWORK_SOURCES})

target_include_directories(network
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${NETWORK_HEADER_DIR}"
)

find_package(OpenSSL REQUIRED)
target_link_libraries(network OpenSSL::SSL)

