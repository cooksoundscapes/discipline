#pragma once
#include "main.h"
#include "audio-utils.h"

inline void MonoText(std::string content, int x, int y, int font_size, Color col) {
    DrawTextEx(font_mono, content.c_str(), (Vector2){(float)x, (float)y}, font_size, 1, col);
}

inline void RegText(std::string content, int x, int y, int font_size, Color col) {
    DrawTextEx(font_regular, content.c_str(), (Vector2){(float)x, (float)y}, font_size, 1, col);
}

inline void DrawOscilloscope(int channel, int x, int y, int w, int h, Color color) {
    auto buff_size = audio->buffer_size();
    if (buff_size > 0) {
        auto buff_data = audio->get_buffer(channel);

        float jump = buff_size / (float)w;
        int i = 0;
        std::vector<Vector2> segments(buff_size);

        for (float s : buff_data) {
            segments[i] = (Vector2){i*jump+x, -(s*(h/2))+y+(h/2)};
            i++;
        }
        DrawLineStrip(segments.data(), buff_size, color);
    }
}

inline void HSlider(Rectangle bounds, const char* name, page::param& p) {
    int name_s = MeasureText(name, 20);
    DrawText(name, bounds.x, bounds.y, 20, RAYWHITE);
    DrawRectangleLines(bounds.x + name_s + 4, bounds.y, bounds.width, bounds.height, RAYWHITE);
    DrawRectangle(
        bounds.x + name_s + 6,
        bounds.y + 2,
        (int)((p.value/(p.max - p.min))*(bounds.width - 4)),
        bounds.height - 4,
        RAYWHITE
    );
}

inline void VSlider(Rectangle bounds, const char* name, page::param& p, std::string postfix) {
    int compensation = (bounds.width - MeasureText(name, 20)) / 2;
    DrawRectangleLines(bounds.x - compensation, bounds.y, bounds.width, bounds.height, RAYWHITE);
    DrawText(name, bounds.x, bounds.y-20, 20, RAYWHITE);
    int value_h = std::max(0.0f, std::min(1.0f, ((p.value - p.min) / (p.max - p.min)))) * bounds.height - 4;

    DrawRectangle(
        bounds.x - compensation + 2,
        bounds.y - 2 + (bounds.height - value_h),
        bounds.width - 4,
        value_h,
        RAYWHITE
    );
    const char* s_val = (f2s(p.value)+postfix).c_str();
    int v_comp = (bounds.width - MeasureText(s_val, 10)) / 2;
    DrawText(s_val, bounds.x - compensation + v_comp, bounds.height+bounds.y+2, 10, RAYWHITE);
}

constexpr int VU_SEGMENTS = 24;

inline Color hueShift(float r) {
    Color c;
    c.b = 0;
    c.a = 0xff;

    // Aplica a escala logarítmica invertida
    float logR = 1.0f - (log10(1.0f + 9.0f * (1.0f - r)) / log10(10.0f));

    if (logR < 0.5) {
        c.r = static_cast<int>(0xff * pow(logR * 2, 1.5)); // Transição mais longa no verde
        c.g = 0xff;
    } else {
        c.r = 0xff;
        c.g = static_cast<int>(0xff * pow((1.0f - logR) * 2, 1.5)); // Transição mais rápida para vermelho
    }

    return c;
}

inline void VUMeterV(float x, float y, int w, float h, int chan) {
    auto rms = RMS(chan);
    DrawRectangleLines  (x, y, w, h, RAYWHITE);

    int segment_h = h / VU_SEGMENTS;
    int n_segments = (rms * h) / segment_h;

    for (int i{0}; i < n_segments; i++) {
        DrawRectangle(x+2, y - 1 + (int)(h - (i+1)*segment_h), w-4, segment_h-1, hueShift((i+1)/(float)VU_SEGMENTS));
    }
}

inline void TextList(std::string list, const std::unordered_map<int, int>& options,int x, int y, int selected, Color text_col, Color rect_col)
{   
    if (options.find(selected) != options.end()) {
        DrawRectangle(x-2, y-2+(options.at(selected) * 22), 56, 20, rect_col);
    }
    RegText(list, x, y, 20, text_col);
}
