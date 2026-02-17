These are the compile instructions for a client-server poker game.


Windows Users
____________________________________________________________________________________________
- You need CMake to compile this program: https://cmake.org/download/
- You also need a C++ compiler: https://www.msys2.org/ https://packages.msys2.org/packages/mingw-w64-x86_64-gcc
    - Open any msys terminal
    - Copy ' pacman -S mingw-w64-x86_64-gcc '
    - close the terminal

- You may need to edit your environment variables:
    - Go to control panel
    - User Accounts -> User Accounts -> Change my environment variables
    - Click the Path variable -> edit -> new
    - Type or select the path of the *bin* folder where your c++ compiler is (C:\msys64\mingw64\bin). This is the default path, but put the path of wherever your msys was installed.
    - Do the same with the cmake folder

- open any terminal

- To check whether Cmake and the compiler was installed correctly run "cmake --version", "g++ --version", and "mingw32-make --version" in your terminal.

- If you've successfully installed everything, run "run.bat" and wait for the project to compile.

- Executable located in the bin folder

Linux Users
___________________________________________________________________________________________________________

- The commands are for debian based distros, but you should be able to look up their equivalents for other distros.

- You need cmake:
 ' sudo apt install cmake '

- You need a C++ compiler:
 ' sudo apt install gcc g++ '

- You may be missing a few additional packages:
 ' sudo apt-get install -y libfreetype6-dev libudev-dev libxrandr-dev libxcursor-dev libxi-dev libvorbis-dev dos2unix libgl1-mesa-dev libflac-dev'
- If anything else is missing:
' sudo apt install build-essential '

- The .csv files may still have the CRLF linefeeds instead of LF. This will cause the program to crash:
 ' dos2unix bin/CardBackPaths.csv bin/CardPaths.csv '

- run the run.sh script and wait for the program to compile.

- Executable located in the bin folder


