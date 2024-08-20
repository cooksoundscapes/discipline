#pragma once
#include <vector>
#include <cmath>

inline std::vector<float> rms_readings(AUDIO_CHANNEL_COUNT, 0);
constexpr float alpha{0.1}; // for EMA (exponential moving average)

inline double RMS(int chan) {
    if (audio->buffer_size() < 1) return 0;
    if (chan < 1 || chan > AUDIO_CHANNEL_COUNT) return 0;

    auto b = audio->get_buffer(chan);

    float last_rms = rms_readings[chan-1];

    if (b.empty()) return 0.0f;
    double rms{0};
    for (float s : b) {
        rms += s * s;
    }
    rms = std::sqrt(rms/b.size()) / 0.74f; // sinewave normalization

    //smoothing
    rms = std::max(0.0, std::min(1.0, alpha * rms + (1 - alpha) * last_rms));
    rms_readings[chan-1] = rms;
    return rms;
}   