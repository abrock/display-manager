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
