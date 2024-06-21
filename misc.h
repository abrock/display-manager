#ifndef MISC_H
#define MISC_H

#include <iostream>
#include <fmt/core.h>

#include <opencv2/core.hpp>

namespace Misc {
template <typename... Args>
inline void println(fmt::format_string<Args...> s, Args&&... args) {
    std::cout << fmt::format(s, std::forward<Args>(args)...) << std::endl;
}

template<class T, int N>
bool is_finite(cv::Vec<T, N> const& val) {
    for (size_t ii = 0; ii < N; ++ii) {
        if (!std::isfinite(val[ii])) {
            return false;
        }
    }
    return true;
}

void msleep(double const t);

/**
 * @brief rgb2rgbw converts from RGB color to RGBW by using the white channel as much as possible.
 * Basic idea:
 * Let's say we have the color (10, 10, 20).
 * Assume for simplicity that a RGBW value of (0,0,0,x) produces the same optical effect as (x,x,x,0).
 * Then we could represent RGB(10, 10, 20) as RGBW(0, 0, 10, 10). We use 10 for white which gives the same
 * impression as RGB (10, 10, 10) and add 10 for the missing blue component.
 *
 * In reality we have for every RGB-color channel a factor which determines how much the white channel
 * triggers the color channel. We call the factors f_r, f_g, and f_b.
 * Example:
 * RGBW(0,0,0,x) looks like RGB(f_r x, f_g x, f_b x).
 * The function figures out how to display a given RGB value using the maximum amount of white possible.
 * This improves the color because the spectrum of the white LED is much smoother than the combined spectrums
 * of the RGB leds.
 *
 * @param rgb
 * @param factors
 * @return
 */
cv::Vec4d rgb2rgbw(cv::Vec3d const& rgb, cv::Vec3d const& factors);

cv::Vec4b rgb2rgbw(cv::Vec3b const& rgb, cv::Vec3d const& factors);

/**
 * @brief strip_prefix removes a given prefix from a string. If the string doesn't start with the given prefix it is returned unchanged.
 * @param str
 * @param prefix
 * @return
 */
std::string strip_prefix(std::string const& str, std::string const& prefix);

/**
 * @brief prune_fn removes the prefix "file://" from a given filename.
 * @param fn
 * @return
 */
std::string prune_fn(std::string const& fn);

std::vector<std::string> split_string(const std::string &in, const char s);

/**
 * @brief trim removes whitespace (according to std::isspace) at the beginning and end of a string.
 * @param input
 * @return
 */
std::string trim(std::string const &input);

/**
 * @brief split_fns splits a string at line breaks (\n) and prunes the individual sub-strings using prune_fn.
 * @param in
 * @param s
 * @return
 */
std::vector<std::string> split_fns(const std::string &in);

}

#endif // MISC_H
