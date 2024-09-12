#include "components.h"

void MonoText(std::string content, int x, int y, int font_size, Color col) {
    DrawTextEx(font_mono, content.c_str(), (Vector2){(float)x, (float)y}, font_size, 1, col);
}

void RegText(std::string content, int x, int y, int font_size, Color col) {
    DrawTextEx(font_regular, content.c_str(), (Vector2){(float)x, (float)y}, font_size, 1, col);
}

void DrawOscilloscope(int channel, int x, int y, int w, int h, Color color) {
    auto buff_size = audio->buffer_size();
    if (buff_size > 0) {
        auto buff_data = audio->get_buffer(channel);
        DrawBuffer(buff_data, x, y, w, h, -1, 1, color);
    }
}

void HSlider(Rectangle bounds, const char* name, page::param& p) {
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

void VSlider(Rectangle bounds, const char* name, page::param& p, std::string postfix) {
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

static constexpr int VU_SEGMENTS = 24;

Color hueShift(float r) {
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

static constexpr double max_db = 0.0;
static constexpr double min_db = -100.0;

void VUMeterV(float x, float y, int w, float h, int chan) {
    auto rms = RMS(chan);

    double decibels;
    if (rms <= 0.0) {
        decibels = -INFINITY;
    } else {
        decibels = 20 * log10(rms);
    }
    // normalization
    decibels = (decibels - min_db) / (max_db - min_db);
    decibels = pow(10, decibels - 1);
    decibels = std::max(0.0, std::min(1.0, decibels));

    DrawRectangleLines  (x, y,  w, h, RAYWHITE);

    int segment_h = h / VU_SEGMENTS;
    int n_segments = (decibels * h) / segment_h;

    for (int i{0}; i < n_segments; i++) {
        DrawRectangle(x+2, y - 1 + (int)(h - (i+1)*segment_h), w-4, segment_h-1, hueShift((i+1)/(float)VU_SEGMENTS));
    }
}

void TextList(std::string list, const std::unordered_map<int, int>& options,int x, int y, int selected, Color text_col, Color rect_col)
{   
    if (options.find(selected) != options.end()) {
        DrawRectangle(x-2, y-2+(options.at(selected) * 22), 56, 20, rect_col);
    }
    RegText(list, x, y, 20, text_col);
}
