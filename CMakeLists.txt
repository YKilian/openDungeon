cmake_minimum_required(VERSION 3.16)
project(openDungeon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_STATIC_LIBRARIES ON CACHE BOOL "Link SFML statically")

include(FetchContent)

FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG master
)
FetchContent_MakeAvailable(SFML)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(openDungeon ${SOURCES})

target_include_directories(openDungeon PUBLIC include)

target_compile_definitions(openDungeon PUBLIC SFML_STATIC)

target_link_libraries(openDungeon
        PRIVATE
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
        nlohmann_json::nlohmann_json
)

if(WIN32)
    add_custom_command(TARGET openDungeon POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/GameMapConfig.json"
            "${CMAKE_CURRENT_BINARY_DIR}/GameMapConfig.json"
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/assets"
            "${CMAKE_CURRENT_BINARY_DIR}/assets"
    )
endif()