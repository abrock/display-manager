#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <stdlib.h>
#include <stdio.h>

#include <opencv2/highgui.hpp>

#include <QObject>
#include <QLabel>
#include <QMouseEvent>

#include <libserial/SerialPort.h>
#include <libserial/SerialPortConstants.h>
#include <libserial/SerialStream.h>

namespace LS = LibSerial;

class DisplayManager : public QObject{
    Q_OBJECT

    std::vector<cv::Mat3b> images;
    size_t image_show_index = 0;

    cv::Mat1b mask;

    /**
     * @brief display_img_scaled scaled an image by a factor of 20 and shows it in the given window name.
     * @param window_name
     * @param img
     */
    static void display_img_scaled(std::string const & window_name, cv::Mat const& img);

    std::mutex image_access;

    /**
     * @brief default_delay_s is the default delay between frames in seconds.
     */
    double default_delay_s = 0.5;

    DisplayManager();

    void image_display_thread();

    void show_image();

    LS::SerialPort port;
    LS::BaudRate connection_baudrate = LS::BaudRate::BAUD_115200;

    bool connect_serial(std::string const& path);

    bool auto_connect_serial();

    void serial_thread();

    std::string serial_line;
    void serial_thread_sub();

    void handle_serial_line(std::string const& line);

public:
    DisplayManager(DisplayManager const&)               = delete;
    void operator=(DisplayManager const&)  = delete;

    static DisplayManager& getInstance() {
        static DisplayManager instance; // Guaranteed to be destroyed.
                                        // Instantiated on first use.
        return instance;
    }


    Q_INVOKABLE void setImageFiles(QString const& str);

    Q_INVOKABLE void setMaskFile(QString const& str);
};

#endif // DISPLAYMANAGER_H
