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

    cv::Vec4b color;

    /**
     * @brief max_num_img is the maximum number of images the uC can store in memory. This is reported by the uC via the serial connection.
     */
    int max_num_img = -1;

    /**
     * @brief num_pixels is the number of pixels according to the uC program. This is reported by the uC via the serial connection.
     */
    int num_pixels = -1;

    int num_frames = 1;

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

    int delay = 1000;

    DisplayManager();

    void image_display_thread();

    void show_image();

    void show_pixel_routine();

    LS::SerialPort port;
    LS::BaudRate connection_baudrate = LS::BaudRate::BAUD_115200;

    bool connect_serial(std::string const& path);

    bool auto_connect_serial();

    std::mutex serial_access;
    void serial_thread();

    void serial_send_line(std::string const& str);

    std::string serial_line;
    void serial_thread_sub();

    void handle_serial_line(std::string const& line);

    /**
     * @brief rgb_factors contains the factors telling the function Misc::rgb2rgbw
     * how much color-intensity the white LED provides for each color-channel.
     */
    cv::Vec3d rgb_factors{1,1,1};

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

    Q_INVOKABLE void setColor(QString const& channel, QString const& value);

    Q_INVOKABLE void setNumFrames(QString const& value);

    Q_INVOKABLE void setDelay(QString const& value);

    /**
     * @brief sendToUC sends a single image to the uC
     * @param img_idx
     */
    void sendImgToUC(int const img_idx);

    Q_INVOKABLE void sendColorToUC();

    Q_INVOKABLE void sendNumFramesToUC();

    Q_INVOKABLE void sendEverythingToUC();

    Q_INVOKABLE void sendDelayToUC();

    /**
     * @brief sendToUC sends the image data to the uC
     */
    Q_INVOKABLE void sendImgToUC();
};

#endif // DISPLAYMANAGER_H
