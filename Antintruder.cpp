#include "Antintruder.h"
#include <cmath>
#include <iostream>

Antintruder::Antintruder(const std::string& file_prefix,
    double intrusion_treshold, double framerate, Interval latching_time, Interval max_part_length) :
    m_file_prefix(file_prefix), m_intrusion_treshold(intrusion_treshold),
    m_framerate(framerate), m_latching_time(latching_time),
    m_max_part_length(max_part_length) {}


Antintruder::~Antintruder()
{
}

void Antintruder::run() {
    cv::Mat frame;

    cv::VideoCapture stream(0);
    if (!stream.isOpened()) {
        return;
    }
    m_video_size = cv::Size(stream.get(CV_CAP_PROP_FRAME_WIDTH), stream.get(CV_CAP_PROP_FRAME_HEIGHT));

    // Initialization
    cv::Mat previous_frame;
    stream >> previous_frame;
    m_previous_frames.push_back(previous_frame);
    while (true) {
        stream >> frame;
        auto time = Time::now();
        process(frame, time);

        // Stop Antintruder
        char c = (char)cv::waitKey(std::ceil(1000 / m_framerate));
        if (c == 27) {
            break;
        }
    }
}

void Antintruder::process(const cv::Mat& frame, const Datetime& time) {
    cv::Mat previous_frame = m_previous_frames.front();
    if (has_intrusion(previous_frame, frame)) {
        m_last_detection = time;
        if (!m_recorder) {
            std::cout << "New intrusion" << std::endl;
            m_recorder = std::make_unique<Recorder>(m_file_prefix, time, m_framerate, m_video_size,
                m_max_part_length);
        }
    }
    if (m_recorder) {
        auto f = m_previous_frames.front();
        m_recorder->record(f, time);
        m_previous_frames.pop_front();
        //m_recorder->record(frame, time);
        //std::cout << (m_latching_time - (Interval)(time - m_last_detection)).count() << std::endl;
    }
    if((Interval)(time - m_last_detection) > m_latching_time && m_recorder) {
        m_recorder.reset();
        std::cout << "End intrusion" << std::endl;
    }
    if (m_previous_frames.size() >= std::max(1., m_framerate * 5)) {
        m_previous_frames.pop_front();
    }
    m_previous_frames.emplace_back();
    frame.copyTo(m_previous_frames.back());
}

bool Antintruder::has_intrusion(const cv::Mat& current_frame, const cv::Mat& previous_frame) {
    cv::Mat diff = cv::abs(previous_frame - current_frame);
    cv::Mat grey;
    cv::imshow("Test", diff);
    auto s = cv::norm(diff, cv::NORM_L2) / m_video_size.width / m_video_size.height;
    return s >= m_intrusion_treshold;
    cv::cvtColor(diff, grey, cv::COLOR_BGR2GRAY);
    cv::Mat thresh;
    cv::threshold(grey, thresh, 63, 255, cv::THRESH_BINARY);
    auto t = cv::norm(thresh, cv::NORM_L2) / m_video_size.width / m_video_size.height;
    return t >= m_intrusion_treshold;
}