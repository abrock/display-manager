#include "misc.h"

#include <opencv2/core.hpp>

#include <glog/logging.h>

namespace Misc {

cv::Vec4d rgb2rgbw(const cv::Vec3d &rgb, cv::Vec3d const& factors) {
    double max_white_value = 255;
    for (size_t ii = 0; ii < 3; ++ii) {
        max_white_value = std::min(max_white_value, rgb[ii] / factors[ii]);
    }
    CHECK_GE(max_white_value, 0);
    CHECK_LE(max_white_value, 255);
    cv::Vec4d result(0, 0, 0, max_white_value);
    for (size_t ii = 0; ii < 3; ++ii) {
        double const already_explained = max_white_value * factors[ii];
        double const missing = rgb[ii] - already_explained;
        CHECK_GE(missing, 0);
        CHECK_LE(missing, 255);
        result[ii] = missing;
    }
    return result;
}

std::string prune_fn(const std::string &fn) {
    return trim(strip_prefix(trim(fn), "file://"));
}

std::string strip_prefix(const std::string &str, const std::string &prefix) {
    if (str.starts_with(prefix)) {
        return str.substr(prefix.size());
    }
    return str;
}

std::vector<std::string> split_string(const std::string &in, const char s) {
    std::string current;
    std::vector<std::string> result;
    for (size_t ii = 0; ii < in.size(); ++ii) {
        if (in[ii] == s) {
            if (current.size() > 0) {
                result.push_back(current);
            }
            current.clear();
        }
        else {
            current += in[ii];
        }
    }
    if (current.size() > 0) {
        result.push_back(current);
    }
    return result;
}

std::string trim(const std::string &input) {
    std::string result(input);
    result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) {
        return !std::isspace(ch);
    }));
    result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) {
        return !std::isspace(ch);
    }).base(), result.end());
    return result;
}

std::vector<std::string> split_fns(const std::string &in) {
    std::vector<std::string> result = split_string(in, '\n');
    for (std::string& fn : result) {
        fn = prune_fn(fn);
    }
    return result;
}

void msleep(const double t) {
    usleep(1'000'000 * t);
}

cv::Vec4b rgb2rgbw(const cv::Vec3b &rgb, const cv::Vec3d &factors) {
    return rgb2rgbw(cv::Vec3d(rgb), factors);
}

}
