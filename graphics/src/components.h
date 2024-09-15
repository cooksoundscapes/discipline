#pragma once
#include "main.h"
#include "audio-utils.h"

extern void MonoText(std::string content, int x, int y, int font_size, Color col);

extern void RegText(std::string content, int x, int y, int font_size, Color col);

template<typename Container>
using ValueType = typename Container::value_type;

template<typename Container>
void DrawBuffer(const Container& buff, int x, int y, int w, int h, double min, double max, Color color) {
    static_assert(std::is_arithmetic<ValueType<Container>>::value, "Container must contain arithmetic types");

    double jump = w / (double)buff.size();
    double range = max - min;
    if (range == 0) return;

    double i = 0.0;
    std::vector<Vector2> segments(buff.size());

    for (const auto& s : buff) {
        float sx = (float)(i*jump+x);
        float ns = (float)((s - min) / range);
        float sy = (float)(y + h - (ns * h));
        segments[i] = (Vector2){sx, sy};
        i++;
    }
    DrawLineStrip(segments.data(), buff.size(), color);
}

extern void DrawOscilloscope(int channel, int x, int y, int w, int h, Color color);

extern void HSlider(Rectangle bounds, const char* name, float level);

extern void HSlider(Rectangle bounds, const char* name, float level, float min, float max);

extern void VSlider(Rectangle bounds, const char* name, float level, float min, float max, std::string postfix);

extern void VSlider(Rectangle bounds, const char* name, float level, std::string postfix);

extern Color hueShift(float r);

extern void VUMeterV(float x, float y, int w, float h, int chan);

extern void TextList(std::string list, const std::unordered_map<int, int>& options,int x, int y, int selected, Color text_col, Color rect_col);