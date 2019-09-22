#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

typedef std::chrono::system_clock Time;
typedef std::chrono::system_clock::time_point Datetime;
typedef std::chrono::duration<double> Interval;

class Recorder
{
    Datetime m_started_at;
    std::string m_file_prefix;
    double m_framerate;
    cv::Size m_video_size;
    Interval m_max_part_length;

public:
    Recorder(const std::string& file_prefix, const Datetime& started_at, double framerate,
        const cv::Size& video_size, Interval max_part_length);
    ~Recorder();
    void record(const cv::Mat& frame, const Datetime& at);
private:
    int m_part;
    Datetime m_part_started_at;
    cv::VideoWriter m_video;
    std::string m_current_file_name;
    std::thread m_ftp_thread;

    void new_part(const Datetime& started_at);
    void send_ftp();
};

