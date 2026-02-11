@echo off

call cleanFramework.bat

echo.
echo ===== Generating all solutions =====

for /d %%D in (*) do (
    if exist "%%D\CMakeLists.txt" (
        echo.
        echo ------------------------------
        echo Generating solution in %%D
        echo ------------------------------

        pushd "%%D"

        if not exist build mkdir build

        rem ANTIGUO CMAKE, TRAERLOS DE VUELTA SI SE DEJA USAR ARMADILLO cmake -S . -B . -G "Visual Studio 17 2022" -A x64
	if /I "%%D"=="Electrotechnics" (
	    cmake -S . -B . -G "Visual Studio 17 2022" -A x64 ^
	      -DCMAKE_TOOLCHAIN_FILE=C:/dev/assets/Armadillo/vcpkg/scripts/buildsystems/vcpkg.cmake
	) else (
	    cmake -S . -B . -G "Visual Studio 17 2022" -A x64
	)
        rem Purge ALL_BUILD from SLN
        for %%S in (*.sln) do (
            powershell -NoLogo -NoProfile -ExecutionPolicy Bypass ^
              -Command "(Get-Content '%%S' -Raw) -replace '(?ms)^Project\([^)]*\)\s*=\s*\"ALL_BUILD\".*?^EndProject\r?\n','' -replace '.*ALL_BUILD\..*\r?\n','' | Set-Content '%%S'"
        )

        rem Remove generated clutter we don't need to keep
        del /f /q CMakeCache.txt 2>nul
        del /f /q cmake_install.cmake 2>nul
        del /f /q *.log 2>nul
        del /f /q *.vcxproj.user 2>nul
        del /f /q ALL_BUILD* 2>nul
        del /f /q ZERO_CHECK* 2>nul

        if exist CMakeFiles rmdir /s /q CMakeFiles
        if exist .vs rmdir /s /q .vs
        for /d %%X in (*.dir) do rmdir /s /q "%%X"

        popd
    )
)

echo.
echo Done.
pause
