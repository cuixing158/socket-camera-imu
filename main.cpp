/**
 * @file        :main.cpp
 * @brief       :PC端接收 IMU 实时数据，通过TCP/IP注册摄像头IP地址，并通过UDP接收IMU数据。
 * @note     :必须和相机处于同一局域网，且固定不变（如 DHCP 设置静态 IP）.允许PC端监听 5555 端口，接收 IMU 数据。功能同my_socket.py文件。输出的时间戳是微秒单位，是从系统相机开启时刻开始的。
 * @details     :PC端通过TCP/IP注册摄像头IP地址，并通过UDP接收IMU数据。接收到的IMU数据是XML格式的字符串，包含多个字段，如陀螺仪、加速度计等。程序解析XML数据并输出到控制台。
 * @date        :2025/06/06 17:14:25
 * @author      :cuixingxing(cuixingxing150@gmail.com)
 * @version     :1.0
 *
 * @copyright Copyright (c) 2025
 *
 */

#include "tinyxml2.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <string> // 放在最前面
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

const std::string CAMERA_IP = "192.168.1.254";
const int TCP_PORT = 3333;
const int UDP_PORT = 5555;

std::map<std::string, long long> parseIMUXML(const std::string& xml)
{
    std::map<std::string, long long> imuData;
    tinyxml2::XMLDocument doc;
    if (doc.Parse(xml.c_str()) == tinyxml2::XML_SUCCESS) {
        tinyxml2::XMLElement* root = doc.FirstChildElement("Function");
        if (root) {
            for (tinyxml2::XMLElement* child = root->FirstChildElement(); child != nullptr; child = child->NextSiblingElement()) {
                const char* text = child->GetText();
                if (text) {
                    imuData[child->Name()] = std::stoll(text);
                }
            }
        }
    } else {
        std::cerr << "[XML parse error]" << std::endl;
    }
    return imuData;
}

void startUDPListener()
{
    int udpSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSock < 0) {
        std::cerr << "UDP socket creation failed" << std::endl;
        return;
    }

    sockaddr_in udpAddr {};
    udpAddr.sin_family = AF_INET;
    udpAddr.sin_port = htons(UDP_PORT);
    udpAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udpSock, (sockaddr*)&udpAddr, sizeof(udpAddr)) < 0) {
        std::cerr << "UDP bind failed" << std::endl;
        return;
    }

    std::cout << "[INFO] UDP listener started on local port " << UDP_PORT << ", waiting for IMU data..." << std::endl;

    char buffer[2048];
    while (true) {
        sockaddr_in senderAddr;
        socklen_t senderLen = sizeof(senderAddr);
        int recvLen = recvfrom(udpSock, buffer, sizeof(buffer) - 1, 0,
            (sockaddr*)&senderAddr, &senderLen);
        if (recvLen > 0) {
            buffer[recvLen] = '\0';
            std::string xml(buffer);

            char ipStr[INET_ADDRSTRLEN] = { 0 };
#ifdef _WIN32
            inet_ntop(AF_INET, &(senderAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
#else
            inet_ntop(AF_INET, &(senderAddr.sin_addr), ipStr, INET_ADDRSTRLEN);
#endif
            // std::cout << "[FROM " << ipStr << ":" << ntohs(senderAddr.sin_port) << "] Received raw XML data:\n"
            //           << xml << std::endl;
            auto imuData = parseIMUXML(xml);
            if (!imuData.empty()) {
                std::cout << "Parsed IMU data: ";
                for (const auto& kv : imuData) {
                    std::cout << kv.first << ": " << kv.second << ", ";
                }
                std::cout << std::endl
                          << std::endl;
            }
        }
    }
}

void startTCPRegistration()
{
    int tcpSock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcpSock < 0) {
        std::cerr << "[ERROR] TCP socket creation failed" << std::endl;
        return;
    }

    sockaddr_in serverAddr {};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(TCP_PORT);
    inet_pton(AF_INET, CAMERA_IP.c_str(), &serverAddr.sin_addr);

    if (connect(tcpSock, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "[ERROR] TCP connection failed: " << strerror(errno) << std::endl;
        return;
    }

    std::cout << "[INFO] TCP connected to camera " << CAMERA_IP << ":" << TCP_PORT << ", IP registered" << std::endl;

    char buf[1024];
    while (true) {
        int len = recv(tcpSock, buf, sizeof(buf), 0);
        if (len <= 0) {
            std::cerr << "[WARN] Camera disconnected TCP connection" << std::endl;
            break;
        }
    }
#ifdef _WIN32
    closesocket(tcpSock);
#else
    close(tcpSock);
#endif
}

int main()
{
#ifdef _WIN32
    WSADATA wsaData;
    int wsaInit = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaInit != 0) {
        std::cerr << "WSAStartup failed: " << wsaInit << std::endl;
        return 1;
    }
#endif

    std::thread udpThread(startUDPListener);
    startTCPRegistration();
    udpThread.join();

#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}
