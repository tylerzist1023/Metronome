@echo off

set PROJ_NAME=metronome
set PROFILE=release
set SRC=src/main.cpp

llvm-rc ./src/resource.rc /FO ./bin/%PROFILE%/resource.o
clang++ -m32 -c -O3 -mavx ./src/main.cpp -I./ext/include -L./ext/lib -o "bin/%PROFILE%/main.o"
clang++ -m32 -c -O3 -mavx ./src/metronome.cpp -I./ext/include -L./ext/lib -o "bin/%PROFILE%/metronome.o"
clang++ -m32 -Wl,-subsystem:windows -o "bin/%PROFILE%/%PROJ_NAME%.exe" "bin/%PROFILE%/main.o" "bin/%PROFILE%/metronome.o"  "bin/%PROFILE%/resource.o" -L./ext/lib -lSDL2_mixer -lSDL2_image -lSDL2_ttf -lSDL2 -luser32

pause