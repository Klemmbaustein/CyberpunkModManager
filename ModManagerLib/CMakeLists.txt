cmake_minimum_required(VERSION 3.15)

project(ModManagerLib)

set(CMAKE_CXX_STANDARD 20)

set(SRC_DIR "Source")

file(
    GLOB_RECURSE
    SRCS
    "${SRC_DIR}/**.cpp"
)

add_subdirectory("../Dependencies/pugixml/" "Dependencies/pugixml")
add_subdirectory("../Dependencies/json/" "Dependencies/json")

if(WIN32)
    set(CURL_USE_SCHANNEL ON)
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory("../Dependencies/curl" "Dependencies/curl")
else()
    find_package(CURL)
    if(CURL_FOUND)
        include_directories(${CURL_INCLUDE_DIR})
    else(CURL_FOUND)
        message(FATAL_ERROR "Could not find the CURL library files.")
    endif(CURL_FOUND)
endif()

add_library(
   ModManagerLib STATIC
   ${SRCS}
)
target_include_directories(ModManagerLib PUBLIC "Include")
target_link_libraries(ModManagerLib PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(ModManagerLib PUBLIC pugixml)
target_link_libraries(ModManagerLib PUBLIC libcurl)

if(NOT WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")
endif()

if (DEFINED TETHER_CI)
    add_compile_definitions(CI_BUILD=${TETHER_CI})
endif()
