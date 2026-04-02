#include "parser_for_BIN_class.hpp"
#include <vector>

void Parser_for_BIN_class::parse(BIN_READER& reader, Data& storage) {

    while (true) {

        char type;
        unsigned char length;

        if (!reader.readBytes(&type, 1)) break;

        if (!reader.readBytes((char*)&length, 1)) break;

        if (type == 1 && length == sizeof(GPS_container)) {
            GPS_container gps;
            if (!reader.readBytes((char*)&gps, length)) break;
            storage.gps.push_back(gps);
        }

        else if (type == 2 && length == sizeof(IMU_container)) {
            IMU_container imu;
            if (!reader.readBytes((char*)&imu, length)) break;
            storage.imu.push_back(imu);
        }

        else {
            vector<char> skip(length);
            if (!reader.readBytes(skip.data(), length)) break;
        }
    }
}