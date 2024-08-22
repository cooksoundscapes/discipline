/**
    PAGINA 1 - GERAL
        K1 GLOBAL BPM
        K2 ?????
        K3 GANHO
        K4 OUTPUT VOLUME
        ***: mais info sobre latencia, interface, inputs...

    PAGINA 2 - MODULAÇÕES
        K1 LFO_1 - NONE / VOLUME / FILTER / DOUBLER / LO-FI
        K2 LFO 2 - idem
        K3 ENV -  NONE / FILTER / LFO1_RATE / LFO1_DEPTH / LFO2_RATE / LFO2_DEPTH
        K4 NOISE -  NONE / VOLUME / FILTER / LO-FI

    PAGINA 2 - DETALHES DE UM MODULADOR (COMO ESCOLHER??)
    LFO:
        RATE (INVALIDATES BPM_RATIO IF TOUCHED)
        DEPTH
        BPM_RATIO (SETS RATE) - UNSET | 1/4 DOT | 1/4 | 1/8 DOT | 1/8 | 1/16 DOT | 1/16
    ENVELOPE:
        K1 GAIN
        K2 DECAY
        K3 MIN
        K4 MAX
    NOISE:
        K1 RATE
        K2 DEPTH
        K3 STEPS (2 - 64 OU SEM REPETIÇÃO)
        K4 SEED

    PAGINA 3 - DRIVE/DEGRADE
        K1 QUANTIZER
        K2 DECIMATOR
        K3 DOUBLER AMOUNT
        K4 DOUBLER DELAY

    PAGINA 4 - FILTER
        K1 BASE CUTOFF
        K2 Q
        K3 TYPE
        K4 ?????

    PAGINA 5 - DELAY
        K1 LEVEL
        K2 FEEDBACK
        K3 TIME (INVALIDATES RATIO IF TOUCHED)
        K4 TIME RATIO (SETS TIME) - 1/4 DOT | 1/4 | 1/8 DOT | 1/8 | 1/16 DOT | 1/16

    PAGINA 5 - DELAY (PRESSIONA DENOVO)
        K1 HIPASS
        K2 LOWPASS
        K3 MODULATION AMT

    botoes 6 7 e 8 por hora ficam a disposição!
    podem ser atalhos customizados, podem ser atalhos fixos,
    podem ser toggles da página
 */

#include "../components.h"

enum PAGE {
    GENERAL = 0,
    MODULATION,
    DEGRADE,
    FILTER,
    DELAY,
    DELAY_OTHER,
    MODULATION_DETAILS_LFO1,
    MODULATION_DETAILS_LFO2,
    MODULATION_DETAILS_ENV,
    MODULATION_DETAILS_NOISE
};

enum MODULATION {
    LFO1 = 0,
    LFO2,
    ENV,
    NOISE
};

enum MOD_TARGET {
    NONE = 0,
    VOLUME,
    FILT_CUTOFF,
    DOUBLER,
    LO_FI,
    LFO_1_RATE,
    LFO_1_DEPTH,
    LFO_2_RATE,
    LFO_2_DEPTH
};

enum WAVEFORM {
    TRI = 0,
    SQR,
    SIN,
    RMP,
    SAW
};

enum BPM_RATIO {
    NOT_SET = 0,
    QUARTER_D,
    QUARTER,
    EIGHT_D,
    EIGHT,
    SIXTEENTH_D,
    SIXTEENTH
};

enum FILT_TYPE {
    LOWPASS = 0,
    HIPASS,
    BANDPASS,
    NOTCH
};

static constexpr int SPACING = 4;
static constexpr int FONT_M = 20;
static constexpr int FONT_S = 10;
static constexpr int MENU_FONT = 18;
static constexpr int PARAM_AREA_W = SCREEN_W - 48;
static constexpr int HEADER_H = 24;
static constexpr int PARAM_AREA_H = SCREEN_H - HEADER_H;
static constexpr int INSTR_IN = 1;
static constexpr int INSTR_OUT = 2;

static const std::unordered_map<int, int> mod_target_opts_lfo1 = {
  {NONE, 0},
  {VOLUME, 1},
  {FILT_CUTOFF, 2},
  {DOUBLER, 3},
  {LO_FI, 4}
};

static const std::unordered_map<int, int> mod_target_opts_lfo2 = {
  {NONE, 0},
  {VOLUME, 1},
  {FILT_CUTOFF, 2},
  {DOUBLER, 3},
  {LO_FI, 4},
  {LFO_1_RATE, 5},
  {LFO_1_DEPTH, 6}
};

static const std::unordered_map<int, int> mod_target_opts_env = {
  {NONE, 0},
  {FILT_CUTOFF, 1},
  {LFO_1_RATE, 2},
  {LFO_1_DEPTH, 3},
  {LFO_2_RATE, 4},
  {LFO_2_DEPTH, 5}
};
static const std::unordered_map<int, int> mod_target_opts_noise = {
  {NONE, 0},
  {VOLUME, 1},
  {FILT_CUTOFF, 2},
  {LO_FI, 3}
};

page fx_stack = {
    {
        {"current-page", {PAGE::GENERAL}},
        {"global-bpm", {120}},
        {"gain", {0, -50, 12}},
        {"master-volume", {0, -50, 12}},
        {"lfo-1-target", {MOD_TARGET::NONE}},
        {"lfo-2-target", {MOD_TARGET::NONE}},
        {"envelope-target", {MOD_TARGET::NONE}},
        {"noise-target", {MOD_TARGET::NONE}},
        {"lfo-1-rate", {0}},
        {"lfo-1-depth", {0}},
        {"lfo-1-wave", {WAVEFORM::TRI}},
        {"lfo-1-ratio", {BPM_RATIO::NOT_SET}},
        {"lfo-2-rate", {0}},
        {"lfo-2-depth", {0}},
        {"lfo-2-wave", {WAVEFORM::TRI}},
        {"lfo-2-ratio", {BPM_RATIO::NOT_SET}},
        {"env-gain", {0}},
        {"env-decay", {0}},
        {"env-min", {0}},
        {"env-max", {0}},
        {"noise-rate", {0}},
        {"noise-depth", {0}},
        {"noise-steps", {0}},
        {"noise-seed", {0}},
        {"quantizer", {0, 0, 1}},
        {"decimator", {0, 0, 1}},
        {"doubler-amt", {0, 0, 1}},
        {"doubler-delay", {0, 0, 1}},
        {"filter-cutoff", {1, 0, 1}},
        {"filter-q", {0.2, 0.05, 1}},
        {"filter-type", {FILT_TYPE::LOWPASS}},
        {"delay-level", {0, 0, 1}},
        {"delay-fdbk", {0, 0, 1}},
        {"delay-time", {0, 0, 1}},
        {"delay-ratio", {BPM_RATIO::EIGHT_D}},
        {"delay-hip", {0, 0, 1}},
        {"delay-lop", {0, 0, 1}},
        {"delay-mod", {0, 0, 1}}
    },
    {
        {"red", hex("#EF3E36")},
        {"ylw", hex("#EDB88B")},
        {"rose", hex("#FAD8D6")},
        {"blu", hex("#17BEBB")},
        {"grey", hex("#2E282A")}
    },
    {
        {"metronome", "metronome.png"}
    },
    [](page::params& params, page::palette& palette, page::textures& textures)
    {
        // 1st the header which is constant
        ClearBackground(BLACK);
        DrawRectangle(0, 0, SCREEN_W, HEADER_H, palette["ylw"]);

        DrawTexture(textures["metronome"], SCREEN_W - 80, 2, WHITE);
        DrawText(f2s(params["global-bpm"]).c_str(), SCREEN_W - 60, SPACING, FONT_M, BLACK);

        DrawText("I", 10, 26, FONT_S, RAYWHITE);
        VUMeterV(SPACING, HEADER_H + 12, 16, 200, INSTR_IN);

        DrawText("O", SCREEN_W - 16, 26, FONT_S, RAYWHITE);
        VUMeterV(SCREEN_W - 20, HEADER_H + 12, 16, 200, INSTR_OUT);

        unsigned int curr_page = floor(params["current-page"]);
        switch(curr_page) {
            case GENERAL: {
                DrawRectangle(2, 2, 34, 20, palette["grey"]);
                MonoText("Gen", 4, 4, MENU_FONT, WHITE);
                MonoText("    Mod Dist Filt Amb", 4, 4, MENU_FONT, BLACK);

                VSlider({32, HEADER_H + 28, 20, 160}, "gain", params["gain"], "dB");
                VSlider({PARAM_AREA_W - 48, HEADER_H + 28, 20, 160}, "master", params["master-volume"], "dB");
                break;
            }
            case MODULATION_DETAILS_LFO1: {
                DrawRectangle(42, 2, 34, 20, palette["grey"]);
                MonoText("    Mod", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen     Dist Filt Amb", 4, 4, MENU_FONT, BLACK);
                unsigned int target = floor(params["lfo-1-target"]);
                switch (target) {
                    case MOD_TARGET::VOLUME: {
                        RegText("LFO 1 (Tremolo) Settings", 32, 32, 24, palette["red"]);
                        RegText("Rate: " + f2s(params["lfo-1-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-1-depth"]) + "dB", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::FILT_CUTOFF: {
                        RegText("LFO 1 (\"Auto Wah\") Settings", 32, 32, 24, palette["ylw"]);
                        RegText("Rate: " + f2s(params["lfo-1-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-1-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::DOUBLER: {
                        RegText("LFO 1 (Chorus) Settings", 32, 32, 24, palette["rose"]);
                        RegText("Rate: " + f2s(params["lfo-1-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-1-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::LO_FI: {
                        RegText("LFO 1 (Lo-Fi) Settings", 32, 32, 24, palette["blu"]);
                        RegText("Rate: " + f2s(params["lfo-1-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-1-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    default: {
                        RegText("LFO 1 Unset", 32, 32, 24, palette["red"]);
                        break;
                    }
                }
                break;
            }
            case MODULATION_DETAILS_LFO2: {
                DrawRectangle(42, 2, 34, 20, palette["grey"]);
                MonoText("    Mod", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen     Dist Filt Amb", 4, 4, MENU_FONT, BLACK);
                unsigned int target = floor(params["lfo-2-target"]);
                switch (target) {
                    case MOD_TARGET::VOLUME: {
                        RegText("LFO 2 (Tremolo) Settings", 32, 32, 24, palette["red"]);
                        RegText("Rate: " + f2s(params["lfo-2-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-2-depth"]) + "dB", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::FILT_CUTOFF: {
                        RegText("LFO 2 (\"Auto Wah\") Settings", 32, 32, 24, palette["ylw"]);
                        RegText("Rate: " + f2s(params["lfo-2-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-2-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::DOUBLER: {
                        RegText("LFO 2 (Chorus) Settings", 32, 32, 24, palette["rose"]);
                        RegText("Rate: " + f2s(params["lfo-2-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-2-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::LO_FI: {
                        RegText("LFO 2 (Lo-Fi) Settings", 32, 32, 24, palette["blu"]);
                        RegText("Rate: " + f2s(params["lfo-2-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-2-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::LFO_1_RATE: {
                        RegText("LFO 2 (LFO 1 Rate) Settings", 32, 32, 24, palette["blu"]);
                        RegText("Rate: " + f2s(params["lfo-2-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-2-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::LFO_1_DEPTH: {
                        RegText("LFO 2 (LFO 1 Depth) Settings", 32, 32, 24, palette["blu"]);
                        RegText("Rate: " + f2s(params["lfo-2-rate"]) + "Hz", 32, 64, 20, RAYWHITE);
                        RegText("Depth: " + f2s(params["lfo-2-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    default: {
                        RegText("LFO 2 Unset", 32, 32, 24, palette["red"]);
                        break;
                    }
                }
                break;
            }
            case MODULATION_DETAILS_ENV: {
                DrawRectangle(42, 2, 34, 20, palette["grey"]);
                MonoText("    Mod", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen     Dist Filt Amb", 4, 4, MENU_FONT, BLACK);
                unsigned int target = floor(params["env-target"]);
                switch (target) {
                    case MOD_TARGET::FILT_CUTOFF: {
                        RegText("Envelope (\"Auto Wah\") Settings", 32, 32, 24, palette["ylw"]);
                        break;
                    }
                    case MOD_TARGET::LFO_1_DEPTH: {
                        RegText("Envelope (LFO 1 - Depth) Settings", 32, 32, 24, palette["blu"]);
                        break;
                    }
                    case MOD_TARGET::LFO_1_RATE: {
                        RegText("Envelope (LFO 1 - Rate) Settings", 32, 32, 24, palette["blu"]);
                        break;
                    }
                    case MOD_TARGET::LFO_2_DEPTH: {
                        RegText("Envelope (LFO 2 - Depth) Settings", 32, 32, 24, palette["red"]);
                        break;
                    }
                    case MOD_TARGET::LFO_2_RATE: {
                        RegText("Envelope (LFO 2 - Rate) Settings", 32, 32, 24, palette["red"]);
                        break;
                    }
                    default:
                        RegText("Envelope Unset", 32, 32, 24, palette["red"]);
                        break;
                }
                if (target > 0) {
                    RegText("Gain: +" + f2s(params["env-gain"]) + "dB", 32, 64, 20, RAYWHITE);
                    RegText("Decay: " + f2s(params["env-decay"]) + " (pow)", 32, 92, 20, RAYWHITE);
                }
                break;
            }
            case MODULATION_DETAILS_NOISE: {
                DrawRectangle(42, 2, 34, 20, palette["grey"]);
                MonoText("    Mod", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen     Dist Filt Amb", 4, 4, MENU_FONT, BLACK);
                unsigned int target = floor(params["noise-target"]);
                switch (target) {
                    case MOD_TARGET::VOLUME: {
                        RegText("Noise Gen. (Slicer) Settings", 32, 32, 24, palette["red"]);
                        RegText("Depth (max): " + f2s(params["noise-depth"]) + "dB", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::FILT_CUTOFF: {
                        RegText("Noise Gen. (Motion Filter) Settings", 32, 32, 24, palette["ylw"]);
                        RegText("Depth (max): " + f2s(params["noise-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    case MOD_TARGET::LO_FI: {
                        RegText("Noise Gen. (Glitch) Settings", 32, 32, 24, palette["blu"]);
                        RegText("Depth (max): " + f2s(params["noise-depth"]) + "%", 32, 92, 20, RAYWHITE);
                        break;
                    }
                    default: {
                        RegText("Noise Gen. Unset", 32, 32, 24, palette["red"]);
                        break;
                    }
                }
                if (target > 0) {
                    RegText("Step Length: " + f2s(params["noise-rate"]) + "ms", 32, 64, 20, RAYWHITE);
                    RegText("Steps: " + f2s(params["noise-steps"]), 32, 124, 20, RAYWHITE);
                    RegText("Seed: " + f2s(params["noise-seed"]), 32, 156, 20, RAYWHITE);
                }
                break;
            }
            case MODULATION: {
                DrawRectangle(42, 2, 34, 20, palette["grey"]);
                MonoText("    Mod", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen     Dist Filt Amb", 4, 4, MENU_FONT, BLACK);

                RegText("LFO 1", 32, HEADER_H+SPACING, 24, RAYWHITE);
                TextList(
                    "None\nVolume\nCutoff\nDoubler\nLo-Fi",
                    mod_target_opts_lfo1,
                    32,
                    HEADER_H+32,
                    params["lfo-1-target"],
                    RAYWHITE,
                    palette["blu"]
                );

                RegText("LFO 2", 32+64, HEADER_H+SPACING, 24, RAYWHITE);
                TextList(
                    "None\nVolume\nCutoff\nDoubler\nLo-Fi\nlfo1 spd\nlfo1 int",
                    mod_target_opts_lfo2,
                    32+64,
                    HEADER_H+32,
                    params["lfo-2-target"],
                    RAYWHITE,
                    palette["red"]
                );

                RegText("Env", 32+128, HEADER_H+SPACING, 24, RAYWHITE);
                TextList(
                    "None\nCutoff\nlfo1 spd\nlfo1 int\nlfo2 spd\nlfo2 int",
                    mod_target_opts_env,
                    32+128,
                    HEADER_H+32,
                    params["env-target"],
                    RAYWHITE,
                    palette["blu"]
                );

                RegText("Noise", 32+192, HEADER_H+SPACING, 24, RAYWHITE);
                TextList(
                    "None\nVolume\nCutoff\nLo-Fi",
                    mod_target_opts_noise,
                    32+192,
                    HEADER_H+32,
                    params["noise-target"],
                    RAYWHITE,
                    palette["red"]
                );
                break;
            }
            case DEGRADE: {
                DrawRectangle(82, 2, 42, 20, palette["grey"]);
                MonoText("        Dist ", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen Mod      Filt Amb", 4, 4, MENU_FONT, BLACK);

                HSlider({32, 32, 90, 16}, "CRUSH", params["quantizer"]);
                HSlider({32, 64, 90, 16}, "LO-FI", params["decimator"]);
                HSlider({32, 96, 90, 16}, "DOUBLER", params["doubler-amt"]);
                HSlider({32, 128, 90, 16}, "SEPARATION", params["doubler-delay"]);
                break;
            }
            case FILTER: {
                DrawRectangle(132, 2, 42, 20, palette["grey"]);
                MonoText("             Filt", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen Mod Dist      Amb", 4, 4, MENU_FONT, BLACK);

                HSlider({32, 32, 90, 16}, "Cutoff", params["filter-cutoff"]);
                HSlider({32, 64, 90, 16}, "Q", params["filter-q"]);
                break;
            }
            case DELAY_OTHER: {
                DrawRectangle(182, 2, 34, 20, palette["grey"]);
                MonoText("                  Amb", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen Mod Dist Filt", 4, 4, MENU_FONT, BLACK);

                HSlider({32, 32, 90, 16}, "HiCut", params["delay-lop"]);
                HSlider({32, 64, 90, 16}, "LoCut", params["delay-hip"]);
                HSlider({32, 96, 90, 16}, "Mod. Amount", params["delay-mod"]);
                break;
            }
            case DELAY: {
                DrawRectangle(182, 2, 34, 20, palette["grey"]);
                MonoText("                  Amb", 4, 4, MENU_FONT, WHITE);
                MonoText("Gen Mod Dist Filt", 4, 4, MENU_FONT, BLACK);

                HSlider({32, 32, 90, 16}, "Level", params["delay-level"]);
                HSlider({32, 64, 90, 16}, "Feedback", params["delay-fdbk"]);
                HSlider({32, 96, 90, 16}, "Time", params["delay-time"]);
                break;
            }
        }
        
    }
};

REGISTER_PAGE(fx_stack);
