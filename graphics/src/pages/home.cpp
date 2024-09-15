#include "../main.h"

enum Submenu {
    NONE = 0,
    JACK_SETTINGS,
    PD_SETTINGS,
    SYSTEM_SETTINGS
};

enum ServiceStatus {
    DEAD = 0,
    RUNNING
};

page home {
    [](page* page){
        ClearBackground(BLACK);
        const char* hi = "HELLO, HANDSOME";
        int t_siz = MeasureText(hi, 20);
        DrawText("HELLO, HANDSOME", (SCREEN_W - t_siz) / 2, SCREEN_H/3, 20, RAYWHITE);
    }
};

REGISTER_PAGE(home)
