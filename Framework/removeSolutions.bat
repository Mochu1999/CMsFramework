@echo off
echo Cleaning Framework directory...

:: Delete all folders (including hidden) except Sources and Resources
for /f "delims=" %%F in ('dir /a:d /b') do (
    if /I not "%%F"=="Sources" if /I not "%%F"=="Resources" (
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

echo Done.
pause
