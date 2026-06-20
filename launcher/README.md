This repository contains a minimal replacement launcher that bypasses the runtime compiler availability check.

Purpose:
- Some original launcher binaries performed an environment check and exited if no C++ compiler was detected in the runtime environment. This small source builds a launcher that does not abort on missing compiler and instead proceeds.

How to build (Windows, recommended):
1. Open "x64 Native Tools Command Prompt for VS 2022" (Developer Command Prompt).
2. Run the build script:
   cd launcher
   build.bat

Or manually:
   mkdir build
   cd build
   cmake -G "Visual Studio 17 2022" -A x64 ..
   cmake --build . --config Release

Notes:
- This replacement is a minimal example. If you want the original functionality preserved, provide the original source files and I will prepare a targeted patch to only disable the compiler-check logic.
