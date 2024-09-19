#include "components.h"

enum Submenu {
    JACK_SETTINGS = 0,
    SYSTEM_SETTINGS
};

enum ServiceStatus {
    DEAD = 0,
    RUNNING
};

static int fsize = 20;
static int fsmall = 16;
static int spc = 4;
static Color txtcol = RAYWHITE;
static int f = 0;

page home {
    [](page* page) {
        f = (f+1) % 90;
        auto& texts = page->text_params;
        auto& params = page->params;

        ClearBackground(BLACK);

        RegText("Audio: ", 2, 2, fsize, txtcol);
        bool audio_started = params["jack-status"] > 0;
        RegText(audio_started ? "Running" : "Stopped", 42, 2, fsize, audio_started ? YELLOW : RED);
        RegText("Pd: ", 100, 2, fsize, txtcol);
        bool pd_started = params["pd-status"] > 0;
        RegText(pd_started ? "OK" : "Dead", 120, 2, fsize, pd_started ? YELLOW : RED);
        RegText("CPU: %" + f2s("cpu-load"), 160, 2, fsize, txtcol);
        RegText("RAM: %" + f2s("ram-usage"), 240, 2, fsize, txtcol);
        
        if (params["jack-start-is-error"] > 0) {
            RegText("Current Settings (Failed):\n" + texts["jack-current-settings"], 2, fsize+spc*3, fsize, RED);
        } else {
            RegText("Current Settings:\n" + texts["jack-current-settings"], 2, fsize+spc*3, fsize, YELLOW);
        }

        DrawLine(0, fsize+spc, SCREEN_W, fsize+spc, txtcol);
        DrawLine(100, fsize*4, 100, SCREEN_H, txtcol);

        VMenu("Audio Cards\nSystem", spc, fsize*4, params["menu-select"], fsize, 96, txtcol, BLUE);
    
        switch((Submenu)params["menu-select"]) {
            case JACK_SETTINGS:
                if (texts["jack-presets"].length() > 0) {
                    VMenu(texts["jack-presets"], 100+spc, fsize*4, params["submenu-select"], fsize, 96, txtcol, RED);
                }
                break;
            case SYSTEM_SETTINGS:
                VMenu("Restart Pd\nReboot\nShutdown", 100+spc, fsize*4, params["submenu-select"], fsize, 96, txtcol, RED);
        }
    }
};

REGISTER_PAGE(home)
