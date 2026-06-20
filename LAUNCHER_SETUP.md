# Launcher Setup & Distribution Guide

## 🎯 Goal
Create a single launcher.exe that users can download and run - that's it! No dependencies needed.

## 📋 Steps

### Step 1: Build the Launcher

```bash
# Run on Windows with Visual Studio 2022 + CMake installed
build-launcher.bat
```

This creates: `build_launcher/bin/launcher.exe`

### Step 2: Prepare skinchanger.exe

You need to:
1. Compile the actual skinchanger application from source
2. Get `skinchanger.exe` (the game modification tool)

### Step 3: Upload to GitHub Releases

1. Go to: https://github.com/cognivar-droid/skinchanger/releases
2. Create a new release tagged `latest`
3. Upload `skinchanger.exe` as a release asset
4. Save/publish the release

### Step 4: Distribute

Share `launcher.exe` with users:
- Users download `launcher.exe`
- Users double-click it
- Launcher automatically:
  - Checks requirements (DirectX 11, CS2)
  - Downloads skinchanger.exe from GitHub
  - Extracts and runs it
  - Done!

## 🔄 Update Process

1. Compile new version of skinchanger.exe
2. Upload to GitHub Release as `latest`
3. Next time users run launcher.exe, it will auto-download the new version

## 📝 Launcher Configuration

Edit `launcher/src/main.cpp` if you want to change:

```cpp
const std::string REPO_OWNER = "cognivar-droid";  // Your username
const std::string REPO_NAME = "skinchanger";      // Your repo name
const std::string DOWNLOAD_URL = "...";           // Download link
```

## ✅ Testing

1. Build launcher: `build-launcher.bat`
2. Upload dummy skinchanger.exe to GitHub Release
3. Run launcher.exe and test:
   - ✓ Checks DirectX
   - ✓ Checks CS2 installed
   - ✓ Downloads application
   - ✓ Launches application

## 🚀 Final Distribution

- **For Users**: Just download `launcher.exe` from Releases
- **For Developers**: Keep source code in GitHub
- **Updates**: Just upload new exe to Releases, users get auto-update
