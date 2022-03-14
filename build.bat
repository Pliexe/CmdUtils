@echo off

echo Building...

echo Building touch.exe
gcc .\src\touch.c -o .\out\touch.exe -std=c99

echo Done.