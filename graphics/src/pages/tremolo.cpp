#include "../components.h"
#include "raygui.h"

page tremolo{
    {
        {"depth", {0.5f, 0.0f, 1.0f}},
        {"waveform", {0, 0, 4, true}},
        {"figure", {0, 0, 3, true}},
        {"rate", {0.5, 0, 1}}
    },
    {
        {"beige", hex("#EAF0CE")},
        {"silver", hex("#C0C5C1")},
        {"slate gray", hex("#7D8491")},
        {"violet light", hex("#574B60")},
        {"violet dark", hex("#3F334D")},
    },
    [](page::params& params, page::palette& palette){
        ClearBackground(BLACK);
        DrawRectangle(0, 0, SCREEN_W, 32, palette["violet dark"]);
        DrawText("Tremolo", 4, 4, 30, RAYWHITE);

        HSlider({10, 48, 70, 15}, "depth", params["depth"]);
        GuiSlider({10, 70, 70, 15}, "rate", nullptr, &params["rate"].value, 0, 1);
    }
};

REGISTER_PAGE(tremolo)