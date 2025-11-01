:: Deletes before building, then deletes unnecessary files
@echo off

echo Cleaning Framework directory...

:: Delete all folders (including hidden) except build, Sources and Resources
for /f "delims=" %%F in ('dir /a:d /b') do (
    if /I not "%%F"=="Sources" if /I not "%%F"=="Resources" if /I not "%%F"=="Offshore" (  REM [ADD] no borrar Offshore
        echo Deleting folder: %%F
        rmdir /S /Q "%%F"
    )
)

:: Delete all files except CMakeLists.txt, removeSolutions.bat, and createSolutions.bat
for %%F in (*) do (
    if /I not "%%F"=="CMakeLists.txt" if /I not "%%F"=="removeSolutions.bat" if /I not "%%F"=="createSolutions.bat" if not exist "%%F\" (
        echo Deleting file: %%F
        del /F /Q "%%F"
    )
)
:: Create a build empty folder
mkdir build

echo Done.

echo Generating Visual Studio solution with CMake...

cmake -S . -B . -G "Visual Studio 17 2022" -A x64

echo Done.

echo Removing generated Visual Studio solution and CMake files...

REM Delete files
del /f /q CMsFramework.vcxproj.user
del /f /q CMakeCache.txt
del /f /q cmake_install.cmake
del /f /q CMsFramework.vcxproj.filters
del /f /q ALL_BUILD.vcxproj
del /f /q ALL_BUILD.vcxproj.filters

REM Delete folder
rmdir /s /q CMakeFiles

echo Done.

echo Purging ALL_BUILD from solution...
powershell -NoLogo -NoProfile -ExecutionPolicy Bypass ^
  -Command "(Get-Content 'CMsFramework.sln' -Raw) -replace '(?ms)^Project\([^)]*\)\s*=\s*\"ALL_BUILD\".*?^EndProject\r?\n','' -replace '.*ALL_BUILD\..*\r?\n','' | Set-Content 'CMsFramework.sln'"
echo Done.


REM ------------------ [ADD] Offshore section ------------------
call :BuildOffshore
pause
goto :eof
REM ------------------------------------------------------------

:BuildOffshore
echo Building Offshore solution in .\Offshore ...

pushd Offshore

echo Cleaning Offshore directory...
for /f "delims=" %%F in ('dir /a:d /b') do (
    if /I not "%%F"=="build" (
        echo Deleting folder: %%F
        rmdir /S /Q "%%F"
    )
)
for %%F in (*) do (
    if /I not "%%F"=="CMakeLists.txt" if not exist "%%F\" (
        echo Deleting file: %%F
        del /F /Q "%%F"
    )
)

mkdir build >nul 2>&1

echo Generating Offshore Visual Studio solution with CMake...
cmake -S . -B . -G "Visual Studio 17 2022" -A x64

echo Removing generated Visual Studio solution and CMake files...
del /f /q Offshore.vcxproj.user 2>nul
del /f /q CMakeCache.txt 2>nul
del /f /q cmake_install.cmake 2>nul
del /f /q ALL_BUILD.vcxproj 2>nul
del /f /q ALL_BUILD.vcxproj.filters 2>nul
rmdir /s /q CMakeFiles 2>nul

echo Purging ALL_BUILD from Offshore.sln...
powershell -NoLogo -NoProfile -ExecutionPolicy Bypass ^
  -Command "(Get-Content 'Offshore.sln' -Raw) -replace '(?ms)^Project\([^)]*\)\s*=\s*\"ALL_BUILD\".*?^EndProject\r?\n','' -replace '.*ALL_BUILD\..*\r?\n','' | Set-Content 'Offshore.sln'"

popd

exit /b
