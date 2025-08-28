cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
project(Poker)


#define variables
set(SFML_DIR "C:/SFML-2.6.1/MinGW (32 bit)")
set(MISC_DIR "C:/Users/terra/Downloads/Projects/Libraries/MinGW/MiscLib")
set(LINKED_LIBS ws2_32 libMisc.a libsfml-graphics.a libsfml-window.a libsfml-audio.a libsfml-main.a libsfml-network.a libopenal32.a libsfml-system.a)
set(INCLUDE_DIRECTORIES ${SFML_DIR}/include ${MISC_DIR}/include)
set(LINK_DIRECTORIES ${SFML_DIR}/lib ${MISC_DIR}/lib)
set(COMPILE_OPTIONS -std=c++20 -O3 -Werror -Wno-trigraphs)

#Build Single Player
file(GLOB SOURCES "src/*.cpp")
add_executable(Poker WIN32 ${SOURCES})
target_compile_options(Poker PRIVATE ${COMPILE_OPTIONS})
target_include_directories(Poker PRIVATE ${INCLUDE_DIRECTORIES})
target_link_directories(Poker PRIVATE ${LINK_DIRECTORIES})
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
target_link_libraries(Poker ${LINKED_LIBS})


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









