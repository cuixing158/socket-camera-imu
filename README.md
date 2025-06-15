# Socket Camera IMU

A C++ project for receiving real-time IMU data from a camera over UDP, and registering the PC's IP address with the camera over TCP/IP.

## Features

- **TCP Registration:** Registers the PC's IP address with the camera via TCP (port 3333).
- **UDP IMU Data Reception:** Listens on UDP port 5555 for real-time IMU data sent by the camera.
- **XML Parsing:** Parses received IMU data in XML format using [tinyxml2](https://github.com/leethomason/tinyxml2).
- **Cross-platform:** Supports both Windows and Linux.

## Requirements

- C++11 or later
- [tinyxml2](https://github.com/leethomason/tinyxml2)
- Windows: Winsock2 (included in Windows SDK)
- Linux: Standard sockets

## Build

### Windows (with LLVM-mingw or MSVC)

1. Make sure `tinyxml2.h` and `tinyxml2.cpp` are in your project or included in your build.
2. Compile with:

    ```sh
    clang++ -std=c++11 -I./tinyxml2 main.cpp tinyxml2.cpp -o imu_reader -lws2_32
    ```

    Or use your preferred CMake/Ninja setup.

### Linux

1. Make sure `tinyxml2.h` and `tinyxml2.cpp` are in your project or included in your build.
2. Compile with:

    ```sh
    g++ -std=c++11 -I./tinyxml2 main.cpp tinyxml2.cpp -o imu_reader -lpthread
    ```

## Usage

1. Ensure your PC and the camera are on the same LAN, and the camera's IP is set to `192.168.1.254` (or modify `CAMERA_IP` in the code).
2. Run the program:

    ```sh
    ./imu_reader
    ```

3. The program will:
    - Register the PC's IP with the camera via TCP.
    - Listen for IMU XML data on UDP port 5555.
    - Parse and print IMU data to the console.

## Output Example

```
[INFO] UDP listener started on local port 5555, waiting for IMU data...
[INFO] TCP connected to camera 192.168.1.254:3333, IP registered
[FROM 192.168.1.254:5555] Received raw XML data:
<Function>
    <timestamp_us>1234567890</timestamp_us>
    <gyro_x>123</gyro_x>
    <gyro_y>456</gyro_y>
    <gyro_z>789</gyro_z>
    ...
</Function>
Parsed IMU data: timestamp_us: 1234567890, gyro_x: 123, gyro_y: 456, gyro_z: 789, ...
```

## Notes

- The program does **not** use `using namespace std;` or `using namespace tinyxml2;` for clarity and safety.
- All console output is in English and uses only ASCII symbols for compatibility.
- If you see socket creation errors on Windows, make sure `WSAStartup` is called (already handled in `main.cpp`).

## License

Copyright (c) 2025  
Author: cuixingxing (<cuixingxing150@gmail.com>)
