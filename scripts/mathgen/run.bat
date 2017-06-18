g++ mathgen.cpp -o mathgen.exe -std=c++17 -Wall -Wextra -pedantic-errors
pause
mathgen.exe
del /F /Q mathgen.exe
g++ math.h -std=c++14 -Wall -Wextra -pedantic-errors 
del /F /Q math.h.gch
pause
move /Y math.h ../../src