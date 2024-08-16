#include "main.h"

void HSlider(Rectangle bounds, const char* name, page::param& p) {
    int name_s = MeasureText(name, 20);
    DrawText(name, bounds.x, bounds.y, 20, RAYWHITE);
    DrawRectangleLines(bounds.x + name_s + 4, bounds.y, bounds.width, bounds.height, RAYWHITE);
    DrawRectangle(
        bounds.x + name_s + 6,
        bounds.y + 2,
        (int)((p.value/p.max)*(bounds.width - 4)),
        bounds.height - 4,
        RAYWHITE
    );
}
