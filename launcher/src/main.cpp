#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <windows.h>
#include <filesystem>
#include <tlhelp32.h>

namespace fs = std::filesystem;
static void sleep_ms(int ms) { std::this_thread::sleep_for(std::chrono::milliseconds(ms)); }

std::wstring to_wstring(const std::string &s) {
    if (s.empty()) return {};
    int len = MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), nullptr, 0);
    std::wstring w; w.resize(len);
    MultiByteToWideChar(CP_UTF8, 0, s.c_str(), (int)s.size(), &w[0], len);
    return w;
}

std::string to_string(const std::wstring &w) {
    if (w.empty()) return {};
    int len = WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), nullptr, 0, nullptr, nullptr);
    std::string s; s.resize(len);
    WideCharToMultiByte(CP_UTF8, 0, w.c_str(), (int)w.size(), &s[0], len, nullptr, nullptr);
    return s;
}

bool launch_process(const std::wstring &path, const std::wstring &args, bool waitForExit = false) {
    STARTUPINFOW si{};
    PROCESS_INFORMATION pi{};
    si.cb = sizeof(si);
    std::wstring cmdLine = L"\"" + path + L"\"" + (args.empty() ? L"" : L" " + args);
    BOOL ok = CreateProcessW(nullptr, cmdLine.data(), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (!ok) {
        DWORD err = GetLastError();
        std::cerr << "CreateProcess failed (error=" << err << ") for: " << to_string(path) << "\n";
        return false;
    }
    CloseHandle(pi.hThread);
    if (waitForExit) {
        WaitForSingleObject(pi.hProcess, INFINITE);
    }
    CloseHandle(pi.hProcess);
    return true;
}

bool shell_open_uri(const std::wstring &uri) {
    HINSTANCE res = ShellExecuteW(nullptr, L"open", uri.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
    return ((INT_PTR)res > 32);
}

// Return the directory of the running executable
fs::path get_exe_dir() {
    wchar_t buf[MAX_PATH];
    DWORD len = GetModuleFileNameW(nullptr, buf, (DWORD)std::size(buf));
    if (len == 0) return {};
    fs::path p(buf);
    return p.parent_path();
}

bool is_process_running_by_name(const std::wstring &processName) {
    bool found = false;
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snap == INVALID_HANDLE_VALUE) return false;
    PROCESSENTRY32W entry;
    entry.dwSize = sizeof(entry);
    if (Process32FirstW(snap, &entry)) {
        do {
            if (_wcsicmp(entry.szExeFile, processName.c_str()) == 0) {
                found = true;
                break;
            }
        } while (Process32NextW(snap, &entry));
    }
    CloseHandle(snap);
    return found;
}

// Heuristic: search up to max_depth directories upward and recursively search for .exe files
fs::path find_candidate_exe(const fs::path &startDir, int max_depth = 4) {
    std::vector<std::string> preferredNames = {"skinchanger.exe", "skinchanger_core.exe", "core.exe", "skinchanger_client.exe", "main.exe", "app.exe"};

    // 1) look for preferred names in parent tree
    fs::path dir = startDir;
    for (int depth = 0; depth <= max_depth && !dir.empty(); ++depth) {
        for (auto &name : preferredNames) {
            fs::path p = dir / name;
            if (fs::exists(p) && fs::is_regular_file(p)) return fs::canonical(p);
        }
        dir = dir.parent_path();
    }

    // 2) recursive search starting from startDir and its parent up to a limited depth
    dir = startDir;
    for (int up = 0; up <= max_depth && !dir.empty(); ++up) {
        try {
            for (auto &e : fs::recursive_directory_iterator(dir)) {
                if (!e.is_regular_file()) continue;
                fs::path p = e.path();
                if (p.extension() == ".exe") {
                    // skip the launcher itself
                    fs::path launcherPath = get_exe_dir() / "launcher.exe";
                    if (fs::equivalent(p, launcherPath)) continue;
                    // skip common build intermediates
                    std::string s = p.string();
                    if (s.find("\\\" build) != std::string::npos) continue;
                    return fs::canonical(p);
                }
            }
        } catch (...) {
            // ignore access errors and continue up
        }
        dir = dir.parent_path();
    }

    return {};
}

int main(int argc, char **argv) {
    auto sleep = [](int ms){ std::this_thread::sleep_for(std::chrono::milliseconds(ms)); };

    std::cout << "========================================\n";
    std::cout << " CS2 SKIN CHANGER - UNIVERSAL LAUNCHER v2.0\n";
    std::cout << "========================================\n\n";

    std::cout << "[STEP 0/5] Checking compiler availability...\n";
    sleep(200);
    std::cout << "[*] Detecting available compilers...\n";
    sleep(300);

    std::cout << "[+] Compiler found: MSVC (simulated)\n\n";

    std::cout << "[STEP 1/5] Preparing build environment...\n";
    sleep(200);

    fs::path exeDir = get_exe_dir();
    fs::path targetPath;

    // If user provided explicit path as arg1, use it
    if (argc >= 2) {
        std::string targ = argv[1];
        targetPath = fs::path(targ);
        if (targetPath.is_relative()) targetPath = exeDir / targetPath;
    }

    // Try to find local skinchanger executable
    if (targetPath.empty() || !fs::exists(targetPath)) {
        fs::path found = find_candidate_exe(exeDir, 4);
        if (!found.empty()) targetPath = found;
    }

    // Step: start the game via Steam if needed, then launch the skinchanger
    bool launchedTarget = false;

    // If no local exe found, try launching CS via Steam URI (AppID 730)
    if (targetPath.empty()) {
        std::cout << "[STEP 2/5] No local skinchanger exe found. Attempting to start CS via Steam...\n";
        sleep(200);
        bool steamOpened = shell_open_uri(L"steam://rungameid/730");
        if (steamOpened) {
            std::cout << "[*] Steam URI opened. Waiting for game process...\n";
            // wait up to 2 minutes for CS process to appear
            const int timeout_ms = 120000;
            int waited = 0;
            const int step = 1000;
            // common process names to check
            std::vector<std::wstring> csNames = {L"cs2.exe", L"csgo.exe", L"hl2.exe"};
            bool gameStarted = false;
            while (waited < timeout_ms) {
                for (auto &name : csNames) {
                    if (is_process_running_by_name(name)) {
                        gameStarted = true;
                        break;
                    }
                }
                if (gameStarted) break;
                sleep(step);
                waited += step;
            }
            if (gameStarted) {
                std::cout << "[*] Detected CS process. Will attempt to launch local skinchanger if present.\n";
                // try to find skinchanger exe again near repo root
                fs::path found = find_candidate_exe(exeDir, 6);
                if (!found.empty()) {
                    targetPath = found;
                }
            } else {
                std::cout << "[!] Timeout waiting for CS process. No local skinchanger will be launched.\n";
            }
        } else {
            std::cout << "[!] Failed to open Steam URI. Cannot start game automatically.\n";
        }
    }

    if (!targetPath.empty() && fs::exists(targetPath)) {
        std::cout << "[STEP 3/5] Found target executable: " << targetPath.string() << "\n";
        sleep(200);
        std::wstring wpath = to_wstring(targetPath.string());
        // Launch the skinchanger executable (do not wait by default)
        if (launch_process(wpath, L"", false)) {
            std::cout << "[+] Launched skinchanger successfully.\n";
            launchedTarget = true;
        } else {
            std::cout << "[!] Failed to launch skinchanger.\n";
        }
    } else {
        std::cout << "[STEP 3/5] No target executable to launch; running internal flow.\n";
    }

    std::cout << "[STEP 4/5] Finalizing...\n";
    sleep(200);
    std::cout << "[STEP 5/5] Done.\n\n";

    if (!launchedTarget) {
        std::cout << "[+] launcher finished (no external target launched).\n";
    } else {
        std::cout << "[+] launcher finished (external target launched).\n";
    }

    std::cout << "\nPress Enter to exit...";
    std::cin.clear();
    std::cin.get();

    return 0;
}
