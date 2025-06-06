cmake_minimum_required(VERSION 3.10)
project(IMU_Reader)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# 添加你的源码
add_executable(imu_reader main.cpp ${CMAKE_SOURCE_DIR}/tinyxml2/tinyxml2.cpp)
target_include_directories(imu_reader PRIVATE ${CMAKE_SOURCE_DIR}/tinyxml2)


if (WIN32)
    target_link_libraries(imu_reader ws2_32)
else()
    target_link_libraries(imu_reader pthread)
endif()