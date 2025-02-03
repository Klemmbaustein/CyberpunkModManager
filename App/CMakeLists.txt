cmake_minimum_required(VERSION 3.15)


set(CMAKE_CXX_STANDARD 23)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Source")
set(MARKUP_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Markup")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(KLEMMUI_INCLUDE_EXAMPLES OFF)

file(
	GLOB_RECURSE
	SRCS
	"${SRC_DIR}/**.cpp"
)

add_subdirectory("../Dependencies/KlemmUI/" "Dependencies/KlemmUI" EXCLUDE_FROM_ALL)
add_subdirectory("../Dependencies/libwebp/" "Dependencies/libwebp" EXCLUDE_FROM_ALL)
add_subdirectory("../ModManagerLib/" "ModManagerLib")

if(WIN32 AND CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
	add_executable(
		ModManager
		${SRCS}
	)
else()
	add_executable(
	   ModManager
	   ${SRCS}
	)
endif()

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

target_link_libraries(ModManager PRIVATE nlohmann_json::nlohmann_json)
target_link_libraries(ModManager PRIVATE KlemmUI)
target_link_libraries(ModManager PRIVATE webp)
target_link_libraries(ModManager PRIVATE ModManagerLib)

klemmui_markup(ModManager "ui")
klemmui_resources(ModManager "res")
