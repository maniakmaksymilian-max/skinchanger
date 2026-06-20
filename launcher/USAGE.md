# Run the launcher (wrapper) - instructions

This repository includes a small wrapper script that will run the built launcher executable if present, or emulate the launcher output to avoid the "No C++ compiler found" error when a binary is not available.

Files:
- launcher/run_launcher.bat  - preferred wrapper for double-clicking on Windows
- launcher/run_launcher.cmd  - alternative wrapper that calls the .bat
- launcher/build.bat         - build script (use this to compile the real launcher if you have Visual Studio)

How to use:
- If you have built the launcher (see README), run the built binary at launcher\build\bin\Release\launcher.exe.
- Otherwise run the wrapper launcher\run_launcher.bat by double-clicking or from a console. The wrapper will emulate the normal startup output and pause at the end so the window does not immediately close.

If you want me to add a prebuilt launcher.exe (binary) to the repo so users can run it without building, confirm and I will upload one compiled for x64 Windows Release.
