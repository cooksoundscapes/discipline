#include "main.h"

/**
 * taking off raygui for now
 * #define RAYGUI_IMPLEMENTATION
 * #include "raygui.h"
 */

#ifdef __linux__
    #include "audio-sink-jack.h"
    #include "osc-server.h"
#else
    #include "audio-sink-dummy.h"
#endif

inline void safe_draw() {
    std::lock_guard<std::mutex> lock(mtx);
    pages[current_page]->draw();
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

    for (auto& page : pages) {
        page.second->load_textures();
    }

    // initialize custom global fonts
    font_mono = LoadFont(MONOSPACE_FONT);
    SetTextureFilter(font_mono.texture, TEXTURE_FILTER_BILINEAR);
    font_regular = LoadFont(REGULAR_FONT);
    SetTextureFilter(font_regular.texture, TEXTURE_FILTER_BILINEAR);

    while (!WindowShouldClose()) {
        BeginDrawing();
        safe_draw();
        EndDrawing();
    }

    for (auto& page : pages) {
        page.second->unload_textures();
    }
    /**
     * Cleanup and exit
     */
    for (auto& pp : pages) {
        pp.second = nullptr;
    }
    audio = nullptr;
    CloseWindow();
    return 0;
}