cmake_minimum_required(VERSION 3.15)

project(ModManagerLib)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR "Source")

file(
    GLOB_RECURSE
    SRCS
    "${SRC_DIR}/**.cpp"
)

add_subdirectory("../Dependencies/json/" "Dependencies/json")

find_package(CURL)

# TODO: use the curl submodule.
# Right now, this will only use pre-installed libcurl.
# To fix this, I would need to figure out compiling it with openssl.
# On windows, CURL compiles with CURL_USE_SCHANNEL. (This will not work on linux)
if(CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
else(CURL_FOUND)
    message(FATAL_ERROR "Could not find the CURL library and development files.")
endif(CURL_FOUND)

add_library(
   ModManagerLib STATIC
   ${SRCS}
)
target_include_directories(ModManagerLib PUBLIC "Include")
target_link_libraries(ModManagerLib PRIVATE nlohmann_json::nlohmann_json)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")

if (DEFINED TETHER_CI)
    add_compile_definitions(CI_BUILD=${TETHER_CI})
endif()
