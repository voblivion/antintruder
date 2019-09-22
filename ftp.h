#pragma once
#include <string>

namespace ftp {
    void send_file(std::string server, int port, std::string user, std::string password, std::string source, std::string target);
}
