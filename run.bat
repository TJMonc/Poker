cd "C:\Users\terra\Downloads\Projects\repos\Poker"
start /wait "" cmake -G "MinGW Makefiles" -D CMAKE_CXX_COMPILER=g++32 -S . -B ./Build 
cd Build
start /wait "" make
cd "C:\Users\terra\Downloads\Projects\repos\Poker"
cd bin
start Poker.exe
