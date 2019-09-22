#include <iostream>
#include <windows.h>
#include <strsafe.h>
#include <wininet.h>
#include <string>
#pragma comment(lib, "wininet.lib")
#pragma comment(lib, "user32.lib")

namespace ftp {
    void send_file(std::string server, int port, std::string user, std::string password, std::string source, std::string target)
    {
        HINTERNET hInternet;
        HINTERNET hFtpSession;
        hInternet = InternetOpen(NULL, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
        if (hInternet == NULL)
        {
            std::cout << "Error 0 : " << GetLastError();
        }
        else
        {
            hFtpSession = InternetConnect(hInternet, server.c_str(), port, user.c_str(), password.c_str(), INTERNET_SERVICE_FTP, 0, 0);
            if (hFtpSession == NULL)
            {
                std::cout << "Error 1 : " << GetLastError();
            }
            else
            {
                if (!FtpPutFile(hFtpSession, source.c_str(), target.c_str(), FTP_TRANSFER_TYPE_BINARY, 0))
                {
                    std::cout << source << std::endl;
                    std::cout << target << std::endl;
                    std::cout << "Error 2 : " << GetLastError();
                }
                else {
                    std::cout << "Sent" << std::endl;
                }
            }
        }
    }
}