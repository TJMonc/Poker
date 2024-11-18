cmake_minimum_required(VERSION 3.21)
add_definitions(-std=c++20)
set(CMAKE_CXX_STANDARD 20)
project(Poker)

file(GLOB SOURCES "src/*.cpp")
include_directories("C:/SFML-2.6.1/Visual Studio (32 bit)/include" "C:/Users/terra/Downloads/Projects/Libraries/MinGW/MiscLib/include")
if(MSVC)
	link_directories("C:/SFML-2.6.1/Visual Studio (32 bit)/lib" "C:/Users/terra/Downloads/Projects/Libraries/Visual Studio/MiscLib/bin/Release")
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/MSVC)
	set(VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
	add_executable(${PROJECT_NAME} ${SOURCES})
	target_link_libraries(${PROJECT_NAME} Misc.lib sfml-graphics.lib sfml-window.lib sfml-audio.lib sfml-main.lib sfml-network.lib openal32.lib sfml-system.lib)
else()
	link_directories("C:/SFML-2.6.1/MinGW (32 bit)/lib" "C:/Users/terra/Downloads/Projects/Libraries/MinGW/MiscLib/lib")
	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)
	add_executable(${PROJECT_NAME} ${SOURCES})
	target_link_libraries(${PROJECT_NAME} libMisc.a libsfml-graphics.a libsfml-window.a libsfml-audio.a libsfml-main.a libsfml-network.a libopenal32.a libsfml-system.a)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_CXX_FLAGS "-Werror -Wno-trigraphs")








