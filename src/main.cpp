#include <lua.hpp>
#include <lo/lo.h>
#include <iostream>

#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

int main() {
    /**
     * OSC setup
     */
    lo_server_thread server = lo_server_thread_new("9000",
    [](int num, const char *msg, const char *path){ // error handler
        std::cout << "OSC server error " << num << " in path " << path << ": " << msg << '\n';
    });
    /**
     * Lua setup
     */
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);
    /**
     * Raylib setup
     */
    InitWindow(640, 480, "testing");
    SetTargetFPS(60);

    float innerRadius = 80.0f;
    float outerRadius = 190.0f;

    float startAngle = 0.0f;
    float endAngle = 360.0f;
    float segments = 0.0f;

    bool drawRing = true;
    bool drawRingLines = false;
    bool drawCircleLines = false;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

            GuiSliderBar((Rectangle){ 600, 40, 120, 20 }, "StartAngle", TextFormat("%.2f", startAngle), &startAngle, -450, 450);
            GuiSliderBar((Rectangle){ 600, 70, 120, 20 }, "EndAngle", TextFormat("%.2f", endAngle), &endAngle, -450, 450);

            GuiSliderBar((Rectangle){ 600, 140, 120, 20 }, "InnerRadius", TextFormat("%.2f", innerRadius), &innerRadius, 0, 100);
            GuiSliderBar((Rectangle){ 600, 170, 120, 20 }, "OuterRadius", TextFormat("%.2f", outerRadius), &outerRadius, 0, 200);

            GuiSliderBar((Rectangle){ 600, 240, 120, 20 }, "Segments", TextFormat("%.2f", segments), &segments, 0, 100);

            GuiCheckBox((Rectangle){ 600, 320, 20, 20 }, "Draw Ring", &drawRing);
            GuiCheckBox((Rectangle){ 600, 350, 20, 20 }, "Draw RingLines", &drawRingLines);
            GuiCheckBox((Rectangle){ 600, 380, 20, 20 }, "Draw CircleLines", &drawCircleLines);

        EndDrawing();
    }
    CloseWindow();
    lua_close(L);
    lo_server_thread_free(server);
    return 0;
}