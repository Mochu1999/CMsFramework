@echo off
echo Cleaning all project subfolders...

for /d %%D in (*) do (
    if exist "%%D\CMakeLists.txt" (
        echo ------------------------------
        echo Cleaning project folder: %%D
        echo ------------------------------

        pushd "%%D"

        rem Remove build directory
        if exist build rmdir /s /q build

        rem Remove CMake junk
        del /f /q CMakeCache.txt 2>nul
        if exist CMakeFiles rmdir /s /q CMakeFiles
        del /f /q cmake_install.cmake 2>nul

        rem Remove solution + project files (but NOT code or CMakeLists)
        for %%F in (*.sln *.vcxproj *.vcxproj.filters *.vcxproj.user) do (
            del /f /q "%%F" 2>nul
        )

        rem Remove Visual Studio temp folders
        if exist .vs rmdir /s /q .vs
        for /d %%X in (*.dir) do rmdir /s /q "%%X"
        if exist .idea rmdir /s /q .idea

        rem DO NOT delete .cpp/.h/.hpp or CMakeLists.txt

        popd
    )
)

echo Done.
pause
