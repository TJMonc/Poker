cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 20)
include(FetchContent)


#set output directories
option(BUILD_SHARED_LIBS "Build shared libraries" ON)
set(FETCHCONTENT_BASE_DIR ${CMAKE_SOURCE_DIR}/deps)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
make_directory(${FETCHCONTENT_BASE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${FETCHCONTENT_BASE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${FETCHCONTENT_BASE_DIR}/lib)

#Fetch content from github
FetchContent_Declare(
    SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.1
)

FetchContent_Declare(
    MiscLib
    GIT_REPOSITORY https://github.com/TJMonc/MiscLib.git
    GIT_TAG 09-06-2025
)
FetchContent_MakeAvailable(SFML)
FetchContent_MakeAvailable(MiscLib)

project(Poker)


#define variables
set(SFML_DIR ${FETCHCONTENT_BASE_DIR})
set(MISC_DIR ${FETCHCONTENT_BASE_DIR})
set(INCLUDE_DIRECTORIES ${SFML_DIR}/sfml-src/include ${MISC_DIR}/misclib-src/include)
set(LINK_DIRECTORIES ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(LINKED_LIBS ws2_32 libMisc.dll.a libsfml-graphics.a libsfml-window.a libsfml-audio.a libsfml-main.a libsfml-network.a libsfml-system.a)
set(COMPILE_OPTIONS -std=c++20 -O3 -Werror -Wno-trigraphs)

#Build Single Player
file(GLOB SOURCES "src/*.cpp")
add_executable(Poker WIN32 ${SOURCES})
target_link_libraries(Poker ${LINKED_LIBS})
target_compile_options(Poker PRIVATE ${COMPILE_OPTIONS})
target_include_directories(Poker PRIVATE ${INCLUDE_DIRECTORIES})
target_link_directories(Poker PRIVATE ${LINK_DIRECTORIES})


#Build Client
file(GLOB SOURCES "cSrc/*.cpp")
add_executable(client WIN32 ${SOURCES})
target_compile_options(client PRIVATE ${COMPILE_OPTIONS})
target_include_directories(client PRIVATE ${INCLUDE_DIRECTORIES})
target_link_directories(client PRIVATE ${LINK_DIRECTORIES})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(client ${LINKED_LIBS})

#Build Server
file(GLOB SOURCES "sSrc/*.cpp")
add_executable(server ${SOURCES})
target_compile_options(server PRIVATE ${COMPILE_OPTIONS})
target_include_directories(server PUBLIC ${INCLUDE_DIRECTORIES})
target_link_directories(server PRIVATE ${LINK_DIRECTORIES})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(server ${LINKED_LIBS})

#Dependencies to ensure build order is correct
set_target_properties(sfml-graphics sfml-window sfml-audio sfml-main sfml-network sfml-system 
PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_dependencies(Misc sfml-graphics sfml-window sfml-audio sfml-main sfml-network sfml-system)
add_dependencies(Poker Misc)



