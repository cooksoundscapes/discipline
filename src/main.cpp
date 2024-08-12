#include <lo/lo.h>

#include <string>
#include <mutex>

#include "main.h"

/**
 * Setup multiple screen mechanism
 */
std::unordered_map<std::string, screen> screens = {
    {HOME, {[](){
        ClearBackground(hex("#2e2e2e"));
        DrawText("HIII BITCHES", SCREEN_W/3, SCREEN_W/3, 20, hex("#fafafa"));
    }, {}}}
};
std::mutex mtx;
std::string current_screen{HOME};

inline void safe_draw() {
    std::lock_guard<std::mutex> lock(mtx);
    screens[current_screen]();
}

int navigate_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    auto next_scrn = &argv[0]->s;
    if (screens.find(next_scrn) != screens.end()) {
        current_screen = next_scrn;
    } else {
        std::cerr << "Screen " << next_scrn << " is not registered;\n";
    }
    return 0;
}

int param_handler(const char* p, const char* t, lo_arg** argv, int argc, lo_message data, void* userData) {
    std::lock_guard<std::mutex> lock(mtx);
    auto key = &argv[0]->s;
    auto value = &argv[1]->f;
    screens[current_screen].set(key, *value);
    return 0;
}

/**
 * Begin main function
 */
int main() {
    /**
     * OSC setup
     */
    lo_server_thread server = lo_server_thread_new(OSC_PORT,
    [](int num, const char *msg, const char *path){
        std::cout << "OSC server error " << num << " in path " << path << ": " << msg << '\n';
    });
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
    CloseWindow();
    lo_server_thread_free(server);
    return 0;
}