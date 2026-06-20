@echo off
REM Build script for launcher (Windows, Developer Command Prompt recommended)
if "%1"=="clean" (
    rd /s /q build
    echo Cleaned build directory.
    goto :eof
)

mkdir build 2>nul
cd build
cmake -G "Visual Studio 17 2022" -A x64 ..
cmake --build . --config Release
cd ..

echo Build finished. Executable is in launcher\build\bin\Release\launcher.exe (or build\bin\Release\launcher.exe)
