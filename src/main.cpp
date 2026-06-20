#include <iostream>
#include <windows.h>
#include "app/application.h"
#include "../injection/injection_engine.h"

int main(int argc, char** argv) {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::cout << "\n";
    std::cout << "  ╔═══════════════════════════════════════════╗" << std::endl;
    std::cout << "  ║     CS2 Skin Changer v2.0 ADVANCED       ║" << std::endl;
    std::cout << "  ║   Advanced Memory-Based Skin Modifier    ║" << std::endl;
    std::cout << "  ║   With Overlay & Inventory Management    ║" << std::endl;
    std::cout << "  ╚═══════════════════════════════════════════╝" << std::endl;
    std::cout << "\n";

    bool injectOnly = false;
    if (argc >= 2) {
        std::string arg = argv[1];
        if (arg == "--inject-only" || arg == "-i") {
            injectOnly = true;
        }
    }

    // Create and initialize application
    Application app;
    
    if (!app.Initialize()) {
        std::cerr << "[Main] Failed to initialize application" << std::endl;
        std::cout << "\nPress any key to exit..." << std::endl;
        getchar();
        return 1;
    }

    if (injectOnly) {
        std::cout << "\n[*] Inject-only mode: performing injection and exiting..." << std::endl;
        // InjectionEngine should have been initialized as part of Application::Initialize (via SkinChangerManager)
        bool injected = InjectionEngine::Inject();
        if (!injected) {
            std::cerr << "[Main] Injection failed" << std::endl;
            app.Shutdown();
            return 2;
        }
        std::cout << "[Main] Injection succeeded" << std::endl;

        // Give a short moment for injected module to initialize
        Sleep(500);

        app.Shutdown();
        return 0;
    }

    std::cout << "\n[*] Starting application..." << std::endl;
    std::cout << "[*] Press INSERT to toggle overlay" << std::endl;
    std::cout << "[*] Press ESC to exit" << std::endl;
    std::cout << "\n" << app.GetStatus() << std::endl;
    std::cout << "\n";

    // Run application
    app.Run();

    // Cleanup
    app.Shutdown();

    return 0;
}
