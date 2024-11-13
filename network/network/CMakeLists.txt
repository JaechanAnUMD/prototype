# Sources
set(NETWORK_SOURCE_DIR src)
set(NETWORK_SOURCES
    ${NETWORK_SOURCE_DIR}/network.cc
)

# Headers
set(NETWORK_HEADER_DIR include)
set(NETWORK_HEADERS
    ${NETWORK_HEADER_DIR}/network.h
)

add_library(network STATIC ${NETWORK_HEADERS} ${NETWORK_SOURCES})

target_include_directories(network
    PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/${NETWORK_HEADER_DIR}"
)

