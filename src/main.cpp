#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <lua.hpp>
#include <lo/lo.h>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <string>
#include <mutex>

#define SCREEN_W 640
#define SCREEN_H 480
#define TARGET_FPS 60
#define OSC_PORT "9000"
#define HOME "home"

/**
 * Inline helper functions
 */
inline Color hex(const char* h) {
    unsigned int r, g, b;
    std::sscanf(h, "#%02x%02x%02x", &r, &g, &b);
    return Color{
        static_cast<unsigned char>(r),
        static_cast<unsigned char>(g),
        static_cast<unsigned char>(b),
        255
    };
}

/**
 * Setup multiple screen mechanism
 */
std::unordered_map<std::string, std::function<void()>> screens = {
    {HOME, [](){
        ClearBackground(hex("#2e2e2e"));
        DrawText("HIII BITCHES", SCREEN_W/3, SCREEN_W/3, 20, hex("#fafafa"));
    }}
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

/**
 * Begin main function
 */
int main() {
    /**
     * OSC setup
     */
    lo_server_thread server = lo_server_thread_new(OSC_PORT,
    [](int num, const char *msg, const char *path){ // error handler
        std::cout << "OSC server error " << num << " in path " << path << ": " << msg << '\n';
    });
    lo_server_thread_add_method(server, "/navigate", "s", navigate_handler, &current_screen);
    lo_server_thread_start(server);
    std::cout << "Listening to OSC messages at port " << OSC_PORT << ";\n";
    /**
     * Lua setup
     */
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    /**
     * Raylib setup
     */
    InitWindow(SCREEN_W, SCREEN_H, "testing");
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
    lua_close(L);
    lo_server_thread_free(server);
    return 0;
}