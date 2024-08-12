#include "../main.h"
#include "raygui.h"

page otoexemplo = {
    [](){
        ClearBackground(BLACK);
        GuiButton((Rectangle){20, 20, 80, 40}, "HAAAHIA");
    }
};

REGISTER_PAGE(otoexemplo)