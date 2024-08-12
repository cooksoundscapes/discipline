#pragma once
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <iostream>
#include <unordered_map>
#include <functional>

#define SCREEN_W 640
#define SCREEN_H 480
#define TARGET_FPS 60
#define OSC_PORT "9000"
#define HOME "home"

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

struct screen {
    std::function<void()> draw;

    std::unordered_map<const char*, float> params;

    void operator()() {
        draw();
    }

    void set(const char* k, float v) {
        if (params.find(k) != params.end()) {
            params[k] = v;
        } else {
            params.insert({k, v});
        }
    }

    float get(const char* k) {
        return params[k];
    }
};