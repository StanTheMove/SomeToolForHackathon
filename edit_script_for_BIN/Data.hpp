#pragma once
#include <vector>

using namespace std;

#pragma pack(push, 1)

struct GPS_container {
    float x;
    float y;
};

struct IMU_container {
    float x1;
    float y1;
    float z1;
};

struct Data {
    vector<GPS_container> gps;
    vector<IMU_container> imu;
};

#pragma pack(pop)