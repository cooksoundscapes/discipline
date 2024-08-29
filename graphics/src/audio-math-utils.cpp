#include <array>
#include <cmath>
#include <complex>
#include "main.h"

static std::array<float, AUDIO_CHANNEL_COUNT> rms_readings = {};
static constexpr float alpha{0.1}; // for EMA (exponential moving average)
static constexpr double smp_rate = 48000.0;

double RMS(int chan) {
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

/**
 * Chamberlin State Variable Filter plotting
 */
static constexpr int num_points = 100;
static constexpr std::array<double, num_points> compute_frequencies() {
    std::array<double, num_points> frequencies = {};
    double f = 10.0;
    for (int i = 0; i < num_points; ++i) {
        frequencies[i] = f;
        f *= 1.1;  // Increment frequency in a logarithmic scale
    }
    return frequencies;
}
static constexpr std::array<double, num_points> frequencies = compute_frequencies();

// Precompute normalized frequencies
static constexpr std::array<double, num_points> compute_omega(const std::array<double, num_points>& freqs, double fs) {
    std::array<double, num_points> omega = {};
    for (int i = 0; i < num_points; ++i) {
        omega[i] = 2 * M_PI * freqs[i] / fs;
    }
    return omega;
}
static constexpr std::array<double, num_points> omega = compute_omega(frequencies, smp_rate);

static void compute_filter_response(std::array<double, num_points>& buffer, double a1, double a2, int filt_type) {
    buffer.fill(0);
    for (int i = 0; i < num_points; ++i) {
        std::complex<double> exp_iw = std::exp(std::complex<double>(0, -omega[i]));
        std::complex<double> numerator;
        switch(filt_type) {
            case 0:
                numerator = 1.0;
                break;
            case 1:
                numerator = -1.0 * exp_iw;
                break;
            case 2:
                numerator = exp_iw - std::exp(std::complex<double>(0, -2 * omega[i])); // For bandpass
                break;
        }
        std::complex<double> denominator = 1.0 + a1 * exp_iw + a2 * std::exp(std::complex<double>(0, -2 * omega[i]));
        buffer[i] = 20 * std::log10(std::abs(1.0 / denominator));
    }
}

void plot_filter(std::array<double, num_points>& buffer, double cutoff, double Q, int filter_type) {
    // coefficients
    const double f = 2 * sin(M_PI * cutoff / smp_rate);
    const double r = 1.0 - 3.0 * Q;
    const double b = f * (1.0 - 0.5 * r);
    const double c = (1.0 - b) / 2.0;
    const double a1 = 1.0 - c;
    const double a2 = -c;

    compute_filter_response(buffer, a1, a2, filter_type);
}