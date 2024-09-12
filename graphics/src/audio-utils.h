#pragma once
#include <array>
#include <complex>

inline constexpr int filter_plot_points = 400;
extern double RMS(int chan);

inline double lin2DB(double linear_value) {
    return 20 * log10(linear_value);
}
inline double lin2DB(std::complex<double>& linear_value) {
    return 20 * log10(std::abs(linear_value));
}

extern void plot_filter(std::array<double, filter_plot_points>& buff, double cutoff, double Q, int filter_type);