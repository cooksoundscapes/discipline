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
    using params = std::unordered_map<std::string, param>;
    using palette = std::unordered_map<std::string, Color>;
    using textures = std::unordered_map<std::string, Texture2D>;
    using texture_refs = std::unordered_map<std::string, std::string>;

    params p_map{};
    palette c_map{};
    textures t_map{};
    texture_refs t_refs{};

    page() = default;

    // 100% stateless drawing
    page(std::function<void()> draw_fn)
    : draw(draw_fn) {}

    // parameters only
    page(params& init_params, std::function<void(params&)> draw_fn)
    : p_map(init_params), draw([this, draw_fn](){ draw_fn(p_map); }) {}

    // palette only
    page(palette colors, std::function<void(palette&)> draw_fn)
    : c_map(colors), draw([this, draw_fn](){ draw_fn(c_map); }) {}

    // parameters & palette
    page(
        params init_params,
        palette colors,
        std::function<void(params&, palette&)> draw_fn
    ) : p_map(init_params), c_map(colors), draw([this, draw_fn](){ draw_fn(p_map, c_map); }) {}

    // parameters & palette & textures
    page(
        params init_params,
        palette colors,
        texture_refs tex_refs,
        std::function<void(params&, palette&, textures&)> draw_fn
    ) : p_map(init_params),
        c_map(colors),
        t_refs(tex_refs),
        draw([this, draw_fn](){ draw_fn(p_map, c_map, t_map); }) {}
    
    std::function<void()> draw;

    void operator()() { 
        draw();
    }

    void load_textures() {
        for (auto& ref : t_refs) {
            t_map.insert({ref.first, LoadTexture(("../assets/" + ref.second).c_str())});
            //SetTextureFilter(t_map[ref.first], TEXTURE_FILTER_ANISOTROPIC_8X);
        }
        t_refs.clear();
    }

    void unload_textures() {
        for (auto& tx : t_map) {
            UnloadTexture(tx.second);
        }
        t_map.clear();
    }

    void set(std::string& k, float v) {
        p_map[k].value = v;
    }
};

/**
 * globals
 */
inline std::unordered_map<std::string, page*> pages{};
inline std::mutex mtx;
inline std::string current_page{HOME};
inline audio_sink* audio = nullptr;
inline std::unordered_map<std::string, std::vector<float>> param_lists{};

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

