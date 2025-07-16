@echo off
echo Generating Visual Studio solution with CMake...

cmake -S . -B . -G "Visual Studio 17 2022" -A x64

echo Done.
pause
