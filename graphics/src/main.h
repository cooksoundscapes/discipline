#pragma once
#include "raylib.h"
#include "audio-sink.h"

#include <iostream>
#include <unordered_map>
#include <functional>
#include <string>
#include <cmath>
#include <mutex>
#include <iomanip>
#include <sstream>

#define SCREEN_W 320
#define SCREEN_H 240
#define TARGET_FPS 60
#define OSC_PORT "7777"
#define HOME "fx_stack"
#define AUDIO_CHANNEL_COUNT 8

/**
 * Inline helper functions
 */
inline Color hex(const char* h) {
    unsigned int r, g, b;
    std::sscanf(h, "#%02x%02x%02x", &r, &g, &b);
    return Color{
        static_cast<unsigned char>(r),
        static_cast<unsigned char>(g),
        static_cast<unsigned char>(b),
        255
    };
}

inline std::string f2s(float f) {
    std::ostringstream r;
    r << std::fixed << std::setprecision(2) << f;
    return r.str();
}

/**
 * page struct definition & page registering macro
 */
struct page {
    struct param {
        float value;
        float min;
        float max;
        bool is_int;
        float pow;
        float mult;
        operator float() {
            return value;
        }
    };
    using p_map = std::unordered_map<std::string, param>;
    using c_map = std::unordered_map<std::string, Color>;
    using txt_map = std::unordered_map<std::string, std::vector<std::string>>;
    using b_map = std::unordered_map<std::string, std::vector<float>>;
    using t_map = std::unordered_map<std::string, Texture2D>;
    using tref_map = std::unordered_map<std::string, std::string>;

    p_map params{};
    c_map palette{};
    txt_map text_params{};
    b_map buffers{};
    t_map textures{};
    tref_map t_refs{};

    page(std::function<void(page*)> draw_fn)
    : draw([this, draw_fn](){ draw_fn(this); }) {}

    // build page with textures, palette and drawing fn. Params will be later initialized
    page(c_map m_palette, tref_map m_textures, std::function<void(page*)> draw_fn)
    : palette(m_palette), t_refs(m_textures), draw([this, draw_fn](){ draw_fn(this); }) {}

    page() = default;
    
    std::function<void()> draw;

    void operator =(std::function<void(page*)> draw_fn) {
        draw = [this, draw_fn]{ draw_fn(this); };
    }

    void operator()() { 
        draw();
    }

    void load_textures() {
        for (auto& ref : t_refs) {
            textures.insert({ref.first, LoadTexture(("../assets/" + ref.second).c_str())});
            //SetTextureFilter(textures[ref.first], TEXTURE_FILTER_ANISOTROPIC_8X);
        }
        t_refs.clear();
    }

    void unload_textures() {
        for (auto& tx : textures) {
            UnloadTexture(tx.second);
        }
        textures.clear();
    }

    void set(std::string& k, float v) {
        if (params.find(k) != params.end()) {
            params[k].value = v;
        } else {
            params.insert({k, {v}});
        }
    }

    void set(std::string& k, std::string v) {
        if (text_params.find(k) != text_params.end()) {
            text_params[k] = {v};
        } else {
            text_params.insert({k, {v}});
        }
    }

    void set(std::string& k, std::vector<std::string> v) {
        if (text_params.find(k) != text_params.end()) {
            text_params[k] = v;
        } else {
            text_params.insert({k, v});
        }
    }

    void set(std::string& k, std::vector<float> v) {
        if (buffers.find(k) != buffers.end()) {
            buffers[k] = v;
        } else {
            buffers.insert({k, v});
        }
    }
};

/**
 * globals
 */
inline std::unordered_map<std::string, page*> pages{};
inline std::mutex mtx;
inline std::string current_page{HOME};
inline audio_sink* audio = nullptr;

// global fonts
#define MONOSPACE_FONT "../assets/fonts/Big Pixel demo.otf"
#define REGULAR_FONT "../assets/fonts/BebasNeue-Regular.ttf"
inline Font font_mono;
inline Font font_regular;

/**
 * Macro for registering pages
 */
#define REGISTER_PAGE(page_name) \
    extern page page_name; \
    static struct Register_##page_name { \
        Register_##page_name() { \
            std::cout << "Registering page " << #page_name << ";\n"; \
            pages[#page_name] = &page_name; \
        } \
    } register_##page_name;

