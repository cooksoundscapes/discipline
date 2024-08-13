#include <lo/lo.h>
#include <mutex>
#include "main.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#ifdef JACK_SINK
    #include "audio-sink-jack.h"
#else
    #include "audio-sink-dummy.h"
#endif

/**
 * Setup multiple page mechanism
 */
std::unordered_map<std::string, page> pages{};
std::mutex mtx;
std::string current_page{HOME};

inline void safe_draw() {
    std::lock_guard<std::mutex> lock(mtx);
    pages[current_page]();
}

int navigate_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    auto next_scrn = &argv[0]->s;
    if (pages.find(next_scrn) != pages.end()) {
        current_page = next_scrn;
    } else {
        std::cerr << "Screen " << next_scrn << " is not registered;\n";
    }
    return 0;
}

int param_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    std::string key = &argv[0]->s;
    float value = argv[1]->f;
    pages[current_page].set(key, value);
    return 0;
}

/**
 * global audio objects
 */
audio_sink_impl snd_inputs(AUDIO_CHANNEL_COUNT);
audio_sink* audio;

/**
 * Begin main function
 */
int main() {
    /**
     * Audio setup
     */
    snd_inputs.start();
    audio = &snd_inputs;

    /**
     * OSC setup
     */
    lo_server_thread server = lo_server_thread_new(OSC_PORT,
    [](int num, const char *msg, const char *path){
        std::cout << "OSC server error " << num << " in path " << path << ": " << msg << '\n';
    });
    if (!server) {
        std::cerr << "Failed to create OSC server.\n";
        return -1;
    }
    lo_server_thread_add_method(server, "/navigate", "s", navigate_handler, nullptr);
    lo_server_thread_add_method(server, "/param", "sf", param_handler, nullptr);
    lo_server_thread_start(server);
    std::cout << "Listening to OSC messages at port " << OSC_PORT << ";\n";

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
    snd_inputs.stop();
    audio = nullptr;
    CloseWindow();
    lo_server_thread_free(server);
    return 0;
}