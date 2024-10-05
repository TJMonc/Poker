cd "C:\Users\terra\Downloads\Projects\repos\PokerProject"
start /wait "" cmake -G "MinGW Makefiles" -S . -B ./Build
cd Build
start /wait "" make
cd "C:\Users\terra\Downloads\Projects\repos\PokerProject"
cd bin
start PokerProject.exe
