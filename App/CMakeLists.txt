cmake_minimum_required(VERSION 3.15)

project(App)

set(CMAKE_CXX_STANDARD 23)

set(SRC_DIR "Source")

file(
    GLOB_RECURSE
    SRCS
    "${SRC_DIR}/**.cpp"
)

add_subdirectory("../Dependencies/KlemmUI/" "Dependencies/KlemmUI")
add_subdirectory("../Dependencies/libwebp/" "Dependencies/libwebp")
add_subdirectory("../ModManagerLib/" "ModManagerLib")

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

add_executable(
   App
   ${SRCS}
)
execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/get-7zip-linux.sh
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies
    RESULT_VARIABLE CMD_ERROR
    OUTPUT_FILE CMD_OUTPUT
)
message(${CMD_ERROR})

if (DEFINED CI_BUILD)
    add_compile_definitions(CI_BUILD=${CI_BUILD})
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic")

target_link_libraries(App PRIVATE SDL2::SDL2)
target_link_libraries(App PRIVATE libglew_static)
target_link_libraries(App PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(App PRIVATE KlemmUI)
target_link_libraries(App PRIVATE webp)
target_link_libraries(App PRIVATE ModManagerLib)
target_link_libraries(App PRIVATE ${CURL_LIBRARIES})
