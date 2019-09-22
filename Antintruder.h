#pragma once

#include <deque>
#include <chrono>
#include <string>
#include <memory>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "Recorder.h"

typedef std::chrono::system_clock Time;
typedef std::chrono::system_clock::time_point Datetime;
typedef std::chrono::duration<double> Interval;

class Antintruder
{
    std::string m_file_prefix;
    double m_intrusion_treshold;
    double m_framerate;
    Interval m_latching_time;
    Interval m_max_part_length;

public:
    Antintruder(const std::string& file_prefix,
        double intrusion_treshold = 0.05f, double framerate = 60.0f,
        Interval latching_time = Interval(30.0), Interval max_part_length = Interval(300.0));
    ~Antintruder();
    void run();
private:
    Datetime m_last_detection;
    cv::Size m_video_size;
    std::unique_ptr<Recorder> m_recorder;
    std::deque<cv::Mat> m_previous_frames;

    void process(const cv::Mat& frame, const Datetime& time);
    bool has_intrusion(const cv::Mat& current_frame, const cv::Mat& previous_frame);
};

