#include "Recorder.h"
#include <iostream>
#include <thread>
#include "ftp.h"

std::string to_string(const Datetime& datetime) {
    auto t = std::chrono::system_clock::to_time_t(datetime);
    struct tm ti;
    localtime_s(&ti, &t);
    char buff[15];
    strftime(buff, 15, "%Y%m%d%H%M%S", &ti);
    return buff;
}

Recorder::Recorder(const std::string& file_prefix, const Datetime& started_at, double framerate,
    const cv::Size& video_size, Interval max_part_length) :
    m_file_prefix(file_prefix),
    m_started_at(started_at),
    m_framerate(framerate),
    m_video_size(video_size),
    m_max_part_length(max_part_length),
    m_part(-1)
{
    new_part(started_at);
}

Recorder::~Recorder()
{
    if (m_part >= 0) {
        m_video.release();
        send_ftp();
        m_ftp_thread.detach();
    }
}

void Recorder::record(const cv::Mat& frame, const Datetime& at) {
    if ((Interval)(at - m_part_started_at) > m_max_part_length) {
        new_part(at);
    }
    m_video.write(frame);
}

void Recorder::new_part(const Datetime& started_at) {
    if (m_part >= 0) {
        m_video.release();
        send_ftp();
    }
    ++m_part;
    m_part_started_at = started_at;
    std::string file_name = m_file_prefix + to_string(m_started_at) + "_" + std::to_string(m_part) + ".avi";
    auto fourcc = CV_FOURCC('H', '2', '6', '4');
    m_video = cv::VideoWriter(file_name, fourcc, m_framerate, m_video_size);
    m_current_file_name = file_name;
}

void Recorder::send_ftp() {
    // Envoyer la vidéo par FTP
    std::string source = std::string("./") + m_current_file_name;
    std::string target = "/records/" + m_current_file_name;
    if (m_ftp_thread.joinable()) {
        m_ftp_thread.join();
    }

    static_assert(false); // must set ftp logins
    m_ftp_thread = std::thread(ftp::send_file, "ftp server", 21, "ftp user", "ftp pass", source, target);
}