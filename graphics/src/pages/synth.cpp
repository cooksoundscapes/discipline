#include "../audio_utils.h"
#include "raygui.h"

page synth{
    {
        {"osc1/level", {1.0f, 0.0f, 1.0f}},
        {"osc1/shape", {0.0f, 0.0f, 1.0f}},
        {"osc1/pitch", {0.0f, 0.0f, 1.0f}},
        {"osc2/level", {0.0f, 0.0f, 1.0f}},
        {"osc2/shape", {0.0f, 0.0f, 1.0f}},
        {"osc2/pitch", {0.0f, 0.0f, 1.0f}},
        {"env/atk", {0.0f, 0.0f, 5000.0f}},
        {"env/dec", {2000.0f, 0.0f, 5000.0f}},
        {"env/sus", {0.2f, 0.0f, 1.0f}},
        {"env/rel", {600.0f, 0.0f, 5000.0f}},
        {"filter/cutoff", {1.0f, 0.0f, 1.0f}},
        {"filter/q", {1.0f, 0.25f, 4.0f}},
    },
    {
        {"bg", hex("#96897B")},
        {"main", hex("#DBAD6A")},
        {"secondary", hex("#628395")},
        {"dark", hex("#CF995F")},
        {"light", hex("#D0CE7C")},
    },
    [](page::params& p, page::palette& palette){
        ClearBackground(RAYWHITE);
        DrawRectangle(0, 0, SCREEN_W, 48, palette["main"]);
    }
};

REGISTER_PAGE(synth)