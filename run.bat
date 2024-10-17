cd "C:\Users\terra\Downloads\Projects\repos\Poker"
cmake -G "MinGW Makefiles" -D CMAKE_CXX_COMPILER="C:/msys64/mingw32/bin/g++.exe" -S . -B ./Build 
cd Build
start /wait "" make
cd "C:\Users\terra\Downloads\Projects\repos\Poker"
cd bin
start Poker.exe
