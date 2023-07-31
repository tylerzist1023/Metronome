@echo off

set PROJ_NAME=metronome
set PROFILE=debug
set SRC=src/main.cpp

windres ./src/resource.rc ./bin/%PROFILE%/resource.o
g++ -c -g ./src/main.cpp -I./ext/include -L./ext/lib -o "bin/%PROFILE%/main.o"
g++ -c -g ./src/metronome.cpp -I./ext/include -L./ext/lib -o "bin/%PROFILE%/metronome.o"
g++ -Wl,-subsystem,windows -o "bin/%PROFILE%/%PROJ_NAME%.exe" "bin/%PROFILE%/main.o" "bin/%PROFILE%/metronome.o"  "bin/%PROFILE%/resource.o" -L./ext/lib -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2 -lSDL2main

pause