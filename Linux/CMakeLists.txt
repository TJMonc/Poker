cmake_minimum_required(VERSION 3.21)
add_definitions(-std=c++20)
set(CMAKE_CXX_STANDARD 20)
project(Poker)

include_directories("/home/terrance/SFML/SFML-2.6.2/include" "/home/terrance/Projects/Libraries/MiscLib/include")
link_directories("/home/terrance/SFML/SFML-2.6.2/lib" "/home/terrance/Projects/Libraries/MiscLib/Linux/lib")

#Build Single Player
file(GLOB SOURCES "src/*.cpp")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(Poker ${SOURCES})
target_link_libraries(Poker Misc sfml-graphics sfml-window sfml-audio sfml-network sfml-system)


#Build Server
file(GLOB SOURCES "sSrc/*.cpp")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
add_executable(server ${SOURCES})
target_link_libraries(server Misc sfml-graphics sfml-window sfml-audio sfml-network sfml-system)
