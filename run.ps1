Set-Location "C:\Users\terra\Downloads\Projects\repos\Poker"
cmake -G "MinGW Makefiles" -D CMAKE_CXX_COMPILER="C:/msys64/mingw32/bin/g++.exe" -D CMAKE_BUILD_TYPE=Release -S . -B ./Build

cmake --build build
Set-Location bin

Start-Process server

Start-Process client
Start-Process client








