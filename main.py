#!/usr/bin/env python
# -*- encoding: utf-8 -*-
"""
@file        :main.py
@description :使用TCP连接相机，并通过UDP实时接收IMU数据！
@date        :2025/06/06 16:15:59
@author      :cuixingxing
@email       :cuixingxing150@gmail.com
@version     :1.0
"""


import socket
import threading
import xml.etree.ElementTree as ET

# 配置
CAMERA_IP = "192.168.1.254"
TCP_PORT = 3333
UDP_PORT = 5555


def parse_imu_xml(xml_string):
    """
    将 XML 字符串解析为 Python 字典
    """
    try:
        root = ET.fromstring(xml_string)
        data = {child.tag: int(child.text) for child in root}
        return data
    except Exception as e:
        print(f"[XML解析错误] {e}")
        return None


def start_udp_listener():
    """
    启动UDP监听器，实时接收IMU数据并解析
    """
    udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    udp_sock.bind(("", UDP_PORT))
    print(f"✅ UDP监听已开启，在本地端口 {UDP_PORT} 等待IMU数据...\n")

    while True:
        data, addr = udp_sock.recvfrom(2048)
        xml_string = data.decode("utf-8", errors="ignore").strip()
        print(f"[来自 {addr}] 接收到原始XML数据:\n{xml_string}")
        imu_data = parse_imu_xml(xml_string)
        if imu_data:
            print(f"👉 解析后的IMU数据: {imu_data}\n")


def start_tcp_registration():
    """
    主动连接相机，告知本机IP（模拟 SocketTest）
    """
    try:
        tcp_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        tcp_sock.connect((CAMERA_IP, TCP_PORT))
        print(f"🌐 已通过TCP连接相机 {CAMERA_IP}:{TCP_PORT}，IP已注册")

        # 等待固件保持连接用，不主动发数据
        while True:
            data = tcp_sock.recv(1024)
            if not data:
                print("⚠️ 相机断开了TCP连接")
                break
    except Exception as e:
        print(f"❌ TCP连接失败: {e}")


if __name__ == "__main__":
    # 启动UDP监听线程
    udp_thread = threading.Thread(target=start_udp_listener, daemon=True)
    udp_thread.start()

    # 主线程用于TCP连接
    start_tcp_registration()
