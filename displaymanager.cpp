#include "displaymanager.h"

#include "misc.h"

#include <thread>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <ParallelTime/paralleltime.h>

void DisplayManager::display_img_scaled(std::string const & window_name, cv::Mat const& img) {
    cv::Mat display;
    cv::resize(img, display, cv::Size(), 20, 20, cv::INTER_NEAREST);
    cv::imshow(window_name, display);
    cv::waitKey(1);
}

DisplayManager::DisplayManager() {
    std::thread (&DisplayManager::image_display_thread, this).detach();
    std::thread (&DisplayManager::serial_thread, this).detach();
}

void DisplayManager::image_display_thread() {
    while (true) {
        ParallelTime t;
        show_image();
        Misc::msleep(std::max(0.2, default_delay_s));
    }
}

void DisplayManager::show_image() {
    std::lock_guard guard(image_access);
    if (images.empty()) {
        return;
    }
    image_show_index++;
    display_img_scaled("image", images.at(image_show_index % images.size()));
    cv::waitKey(1);
}

bool DisplayManager::connect_serial(std::string const& path) {
    std::lock_guard guard(serial_access);
    try {
        port.Open(path);
        port.SetBaudRate(connection_baudrate);
    }
    catch(...) {
        port.Close();
        return false;
    }
    return port.IsOpen();
}

bool DisplayManager::auto_connect_serial() {
    for (std::string const& path : {
         "/dev/ttyUSB0", "/dev/ttyUSB1", "/dev/ttyACM0", "/dev/ttyACM1"
}) {
        if (connect_serial(path)) {
            Misc::println("Serial connected at {}", path);
            return true;
        }
    }
    return false;
}

void DisplayManager::serial_thread() {
    while (true) {
        serial_thread_sub();
        Misc::msleep(.1);
    }
}

void DisplayManager::serial_send_line(const std::string &str) {
    std::lock_guard guard(serial_access);
    port.Write(str);
    port.DrainWriteBuffer();
    static size_t sent_counter = 0;
    sent_counter++;
    Misc::println("Sent line #{} to uC", sent_counter);
}

void DisplayManager::serial_thread_sub() {
    if (!port.IsOpen()) {
        if (!auto_connect_serial()) {
            return;
        }
    }
    try {
        std::lock_guard guard(serial_access);
        while (port.IsDataAvailable()) {
            char c = 0;
            port.ReadByte(c, 1);
            if (c == 0) {
                break;
            }
            if ('\n' == c || '\r' == c) {
                if (!serial_line.empty()) {
                    handle_serial_line(serial_line);
                }
                serial_line.clear();
            }
            else {
                serial_line += c;
            }
        }
    }
    catch (std::exception const& e) {
        Misc::println("Got exception while reading serial data: \n{}\n", e.what());
    }
}

void DisplayManager::handle_serial_line(const std::string &line) {
    static size_t line_counter = 0;
    line_counter++;
    Misc::println("Got line #{} from uC: \n{}\n", line_counter, line);
    if (line.starts_with("max-num-img: ")) {
        int const old_val = max_num_img;
        max_num_img = std::stoi(line.substr(std::string("max-num-img: ").size()));
        if (old_val != max_num_img) {
            Misc::println("Maximum number of images reported by uC: {}", max_num_img);
        }
        return;
    }
    if (line.starts_with("num-pixels: ")) {
        int const old_val = num_pixels;
        num_pixels = std::stoi(line.substr(std::string("num-pixels: ").size()));
        if (old_val != num_pixels) {
            Misc::println("Number of pixels reported by uC: {}", num_pixels);
        }
        return;
    }
}

void DisplayManager::setImageFiles(const QString &str) {
    std::lock_guard guard(image_access);

    std::vector<std::string> files = Misc::split_fns(str.toStdString());
    images.clear();
    for (std::string const& fn : files) {
        cv::Mat3b const img = cv::imread(fn, cv::IMREAD_COLOR);
        if (img.empty()) {
            Misc::println("Warning: image file {} couldn't be loaded", fn);
        }
        else {
            Misc::println("Loaded image {} with size {}x{}", fn, img.size().width, img.size().height);
            images.push_back(img);
        }
    }
}

void DisplayManager::setMaskFile(const QString &str) {
    std::lock_guard guard(image_access);

    mask = cv::imread(Misc::prune_fn(str.toStdString()), cv::IMREAD_GRAYSCALE);
    Misc::println("Mask image: {}x{}", mask.size().width, mask.size().height);
    if (!mask.empty()) {
        display_img_scaled("Mask", mask);
    }
}

cv::Point bottom_left(cv::Size const& size) {
    return {0, size.height-1};
}

cv::Point bottom_left(cv::Mat const& img) {
    return bottom_left(img.size());
}

bool in_img(cv::Size const& size, cv::Point const& pt) {
    return pt.x >= 0 && pt.y >= 0 && pt.x < size.width && pt.y < size.height;
}

bool in_img(cv::Mat const& img, cv::Point const& pt) {
    return in_img(img.size(), pt);
}

bool step(cv::Mat const& img, cv::Point& current_pos, cv::Point& direction) {
    if (!in_img(img, current_pos)) {
        return false;
    }
    if (in_img(img, current_pos + direction)) {
        current_pos += direction;
        return true;
    }
    cv::Point const line_up{0,-1};
    if (in_img(img, current_pos + line_up)) {
        current_pos += line_up;
        direction *= -1;
        return true;
    }
    return false;
}

void DisplayManager::sendToUC(const int img_idx) {
    if (img_idx >= max_num_img || img_idx >= images.size()) {
        return;
    }
    std::string msg = "Set image #" + std::to_string(img_idx);

    cv::Mat3b const& img = images[img_idx];
    if (mask.size != img.size) {
        return;
    }

    cv::Point current_pos = bottom_left(img);
    cv::Point current_direction {1,0};
    for (int ii = 0; ii < num_pixels; ++ii) {
        if (mask(current_pos) < 128) {
            step(img, current_pos, current_direction);
            continue;
        }
        msg += char(img(current_pos)[1]);
        if (!step(img, current_pos, current_direction)) {
            break;
        }
    }
    serial_send_line(msg);
}

void DisplayManager::sendToUC() {
    for (int img_idx = 0; img_idx < images.size() && img_idx < max_num_img; ++img_idx) {
        sendToUC(img_idx);
    }
}
