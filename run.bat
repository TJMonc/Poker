cd "C:\Users\terra\Downloads\Projects\repos\Poker"
start /wait "" cmake -G "MinGW Makefiles" -S . -B ./Build
cd Build
start /wait "" make
cd "C:\Users\terra\Downloads\Projects\repos\Poker"
cd bin
start PokerProject.exe
