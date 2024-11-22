# Sources
set(NETWORK_SOURCE_DIR src)
set(NETWORK_SOURCES
    ${NETWORK_SOURCE_DIR}/network.cc
    ${NETWORK_SOURCE_DIR}/global.cc
    ${NETWORK_SOURCE_DIR}/socket.cc
    ${NETWORK_SOURCE_DIR}/postgres.cc
)

# Headers
set(NETWORK_HEADER_DIR include)
set(NETWORK_HEADERS
    ${NETWORK_HEADER_DIR}/network.h
    ${NETWORK_HEADER_DIR}/util.h
    ${NETWORK_HEADER_DIR}/socket.h
    ${NETWORK_HEADER_DIR}/merkle_tree.h
    ${NETWORK_HEADER_DIR}/postgres.h
)

add_library(network STATIC ${NETWORK_HEADERS} ${NETWORK_SOURCES})

target_include_directories(network
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${NETWORK_HEADER_DIR}"
)

find_package(OpenSSL REQUIRED)
find_package(PostgreSQL REQUIRED)

target_include_directories(network PRIVATE ${PostgreSQL_INCLUDE_DIRS})

target_link_libraries(network PUBLIC PostgreSQL::PostgreSQL OpenSSL::SSL)
