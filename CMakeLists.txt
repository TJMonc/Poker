cmake_minimum_required(VERSION 3.21)
add_definitions(-std=c++20)
set(CMAKE_CXX_STANDARD 20)
project(Poker)

#Build Single Player
file(GLOB SOURCES "src/*.cpp")
include_directories("C:/SFML-2.6.1/Visual Studio (32 bit)/include" "C:/Users/terra/Downloads/Projects/Libraries/MinGW/MiscLib/include")
link_directories("C:/SFML-2.6.1/MinGW (32 bit)/lib" "C:/Users/terra/Downloads/Projects/Libraries/MinGW/MiscLib/lib")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(Poker ${SOURCES})
target_link_libraries(Poker ws2_32 libMisc.a libsfml-graphics.a libsfml-window.a libsfml-audio.a libsfml-main.a libsfml-network.a libopenal32.a libsfml-system.a)


#Build Client
file(GLOB SOURCES "cSrc/*.cpp")
include_directories("C:/SFML-2.6.1/Visual Studio (32 bit)/include" "C:/Users/terra/Downloads/Projects/Libraries/MinGW/MiscLib/include")
link_directories("C:/SFML-2.6.1/MinGW (32 bit)/lib" "C:/Users/terra/Downloads/Projects/Libraries/MinGW/MiscLib/lib")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(client ${SOURCES})
target_link_libraries(client ws2_32 libMisc.a libsfml-graphics.a libsfml-window.a libsfml-audio.a libsfml-main.a libsfml-network.a libopenal32.a libsfml-system.a)

#Build Server
file(GLOB SOURCES "sSrc/*.cpp")
link_directories("C:/SFML-2.6.1/MinGW (32 bit)/lib" "C:/Users/terra/Downloads/Projects/Libraries/MinGW/MiscLib/lib")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(server ${SOURCES})
target_link_libraries(server ws2_32 libMisc.a libsfml-graphics.a libsfml-window.a libsfml-audio.a libsfml-main.a libsfml-network.a libopenal32.a libsfml-system.a)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-Werror -Wno-trigraphs")








