#pragma once
#include <cstdint>
#include <vector>

//контейнери для GPS та IMU даних
struct GPSdata {
    double timestamp;
    double latitude;
    double longitude;
    double altitude; 
};

struct IMUdata {
    double timestamp;
    double accelX;
    double accelY;
    double accelZ;
};
// загальне сховище (структура для зберігання всіх даних)
struct Data {
    std::vector<GPSdata> gps;
    std::vector<IMUdata> imu;
};