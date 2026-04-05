#include "parser_for_BIN_class.hpp"
#include <map>
#include <string>
#include <iostream>
#include <cstring> 

#define HEADER1 0xA3
#define HEADER2 0x95

using namespace std;

#pragma pack(push, 1)
struct RawIMU{
    uint64_t TimeUS;
    float GyrX, GyrY, GyrZ;
    float AccX, AccY, AccZ;
};

struct RawGPS{
    uint8_t Status;
    uint64_t TimeUS;
    uint16_t GWk;
    uint32_t GMs;
    int32_t Lat;
    int32_t Lng;
    float Alt;
    float Spd;
    float GCrs;
    float VZ;
    uint8_t U;
};
#pragma pack(pop)

void Parser_for_BIN_class::parse(BIN_READER& reader, Data& storage) {

    map<int, string> msgTypes;
    map<int, int> msgLengths;

    unsigned char byte;

    while (reader.readBytes((char*)&byte, 1)) {

        if (byte != HEADER1) continue;

        if (!reader.readBytes((char*)&byte, 1)) break;
        if (byte != HEADER2) continue;

        unsigned char msg_id;
        if (!reader.readBytes((char*)&msg_id, 1)) break;

        if (msg_id == 0x80) {

            char buffer[64];

            if (!reader.readBytes(buffer, sizeof(buffer)))
                break;

            int id = (unsigned char)buffer[0];
            int length = (unsigned char)buffer[1];
            string name(buffer + 2, 4);

            msgTypes[id] = name;
            msgLengths[id] = length;

            cout << "FMT: ID=" << id << " NAME=" << name << " LEN=" << length << endl;
        }

        else {

            if (msgTypes.find(msg_id) == msgTypes.end()) {
            continue;
            }

            string type = msgTypes[msg_id];
            int len = msgLengths[msg_id];

            if (type.find("GPS") != string::npos) {

                vector<char> buffer(len);

                if (!reader.readBytes(buffer.data(), len))
                    break;
                if (len >= sizeof(RawGPS)) {
                    RawGPS* rawGps = reinterpret_cast<RawGPS*>(buffer.data());

                    GPSdata cleanPoint;
                    cleanPoint.timestamp = rawGps->TimeUS / 1e6;
                    cleanPoint.latitude = rawGps->Lat / 1e7;
                    cleanPoint.longitude = rawGps->Lng / 1e7;
                    cleanPoint.altitude = rawGps->Alt;

                    storage.gps.push_back(cleanPoint);
                }
            }

            else if (type.find("IMU") != string::npos) {

                vector<char> buffer(len);

                if (!reader.readBytes(buffer.data(), len))
                    break;

                if (len >= sizeof(RawIMU)) {
                    RawIMU* rawImu = reinterpret_cast<RawIMU*>(buffer.data());

                    IMUdata cleanRecord;
                    cleanRecord.timestamp = rawImu->TimeUS / 1e6;
                    cleanRecord.accelX = rawImu->AccX;
                    cleanRecord.accelY = rawImu->AccY;
                    cleanRecord.accelZ = rawImu->AccZ;

                    storage.imu.push_back(cleanRecord);
                }
            }

            else {
                vector<char> skip(len);
                reader.readBytes(skip.data(), len);
            }
        }
    }
}