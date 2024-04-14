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

void DisplayManager::serial_thread_sub() {
    if (!port.IsOpen()) {
        if (!auto_connect_serial()) {
            return;
        }
    }
    try {
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
    Misc::println("Got line from uC: \n{}\n", line);
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
