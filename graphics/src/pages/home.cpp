#include "../main.h"

page home {
    [](){
        ClearBackground(BLACK);
        const char* hi = "HELLO, HANDSOME";
        int t_siz = MeasureText(hi, 20);
        DrawText("HELLO, HANDSOME", (SCREEN_W - t_siz) / 2, SCREEN_H/3, 20, RAYWHITE);
    }
};

REGISTER_PAGE(home)
