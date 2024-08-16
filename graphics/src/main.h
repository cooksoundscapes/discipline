#pragma once
#include "raylib.h"
#include "audio-sink.h"

#include <iostream>
#include <unordered_map>
#include <functional>
#include <string>
#include <cmath>
#include <mutex>

#define SCREEN_W 320
#define SCREEN_H 240
#define TARGET_FPS 60
#define OSC_PORT "7777"
#define HOME "home"
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
    };
    using params = std::unordered_map<std::string, param>;
    using palette = std::unordered_map<std::string, Color>;

    params p_map{};
    palette c_map{};

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
    
    std::function<void()> draw;

    void operator()() { 
        draw();
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

