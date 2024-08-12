#include "../main.h"

page home {
    [](){
        ClearBackground(DARKGRAY);
        DrawText("HELLO, HANDSOME", SCREEN_W/3, SCREEN_H/3, 20, RAYWHITE);
    }
};

REGISTER_PAGE(home)