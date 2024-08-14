#include "main.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#ifdef __linux__
    #include "audio-sink-jack.h"
    #include "osc-server.h"
#else
    #include "audio-sink-dummy.h"
#endif

/**
 * globals
 */
std::unordered_map<std::string, page> pages{};
std::mutex mtx;
std::string current_page{HOME};
audio_sink* audio = nullptr;

inline void safe_draw() {
    std::lock_guard<std::mutex> lock(mtx);
    pages[current_page]();
}

/**
 * Begin main function
 */
int main() {
    #ifdef __linux__
        osc_server serv;
    #endif
    audio_sink_impl snd_inputs(AUDIO_CHANNEL_COUNT);
    audio = &snd_inputs;

    if (pages.size() < 1) {
        std::cerr << "Page registration macro not functioning properly;\nAs there's no available page to render, program will exit;\n";
        return 1;
    }
 
    /**
     * Raylib setup
     */
    InitWindow(SCREEN_W, SCREEN_H, "Discipline");
    SetTargetFPS(TARGET_FPS);

    while (!WindowShouldClose()) {
        BeginDrawing();
        safe_draw();
        EndDrawing();
    }

    /**
     * Cleanup and exit
     */
    audio = nullptr;
    CloseWindow();
    return 0;
}