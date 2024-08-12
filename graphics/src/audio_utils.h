#include "main.h"

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

inline double RMS(std::vector<float> b) {
    if (b.empty()) return 0.0f;
    double rms{0};
    for (float s : b) {
        rms += s * s;
    }
    return std::sqrt(rms/b.size());
}