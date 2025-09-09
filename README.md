
Windows Users
____________________________________________________________________________________________
- You need CMake to compile this program: https://cmake.org/download/
- You also need a C++ compiler: https://www.msys2.org/ https://packages.msys2.org/packages/mingw-w64-x86_64-gcc

- You may need to edit your environment variables:
    - Go to control panel
    - User Accounts -> User Accounts -> Change my environment variables
    - Click the Path variable -> edit -> new
    - Type or select the path of the *bin* folder where your c++ compiler is (Where all the executables and dlls are).
    - Do the same with the cmake folder

- To check whether Cmake and the compiler was installed correctly run "cmake --version", "g++ --version", and "mingw32-make --version" in your terminal.

- If you've successfully installed everything, run "run.bat" and wait for the project to compile.

Linux Users
___________________________________________________________________________________________________________

- The commands in this is for debian based distros, but you should be able to look up their equivalents in other distros.

- You need cmake:
 ' sudo apt install cmake '

- You need a C++ compiler:
 ' sudo apt install gcc g++ '

- You may be missing a few additional packages:
 ' sudo apt-get install -y libfreetype6-dev libudev-dev libxrandr-dev libxcursor-dev libxi-dev libvorbis-dev dos2unix '

- The .csv files may still have the CRLF linefeeds instead of LF. This will cause the program to crash:
 ' dos2unix bin/CardBackPaths.csv bin/CardPaths.csv '

- run the run.sh script and wait for the program to compile.

