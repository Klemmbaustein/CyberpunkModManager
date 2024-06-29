cmake_minimum_required(VERSION 3.15)


set(CMAKE_CXX_STANDARD 23)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(MARKUP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Markup")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

file(
    GLOB_RECURSE
    SRCS
    "${SRC_DIR}/**.cpp"
)

add_subdirectory("../Dependencies/KlemmUI/" "Dependencies/KlemmUI" EXCLUDE_FROM_ALL)
add_subdirectory("../Dependencies/KlemmUI/Markup" "Dependencies/bin" EXCLUDE_FROM_ALL)
add_subdirectory("../Dependencies/libwebp/" "Dependencies/libwebp" EXCLUDE_FROM_ALL)
add_subdirectory("../ModManagerLib/" "ModManagerLib")

add_executable(
   ModManager
   ${SRCS}
)

if(NOT WIN32)
    execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies/get-7zip-linux.sh
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/../Dependencies
        RESULT_VARIABLE CMD_ERROR
        OUTPUT_FILE CMD_OUTPUT
    )
endif()

if (DEFINED CI_BUILD)
    add_compile_definitions(CI_BUILD=${CI_BUILD})
endif()

add_custom_target(
    HeaderBuild
    COMMENT "Running KlemmUI Markup header tool..."
    COMMAND $<TARGET_FILE:KlemmUIHT> -i ${MARKUP_DIR} -o ${SRC_DIR}/Markup
)
add_dependencies(HeaderBuild KlemmUIHT)

add_dependencies(ModManager HeaderBuild)

target_link_libraries(ModManager PRIVATE SDL2::SDL2)
target_link_libraries(ModManager PRIVATE libglew_static)
target_link_libraries(ModManager PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(ModManager PRIVATE KlemmUI)
target_link_libraries(ModManager PRIVATE webp)
target_link_libraries(ModManager PRIVATE ModManagerLib)
