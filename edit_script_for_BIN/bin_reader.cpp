#include <iostream>
#include <fstream> 
#include <vector>
#include "BIN_class_struct.hpp"
#include "Parser_for_BIN_class.hpp"

using namespace std;

#pragma pack(push, 1)

// структура контейнерів для збереження кординат (GPS)
struct GPS_container {
    float x;
    float y;
};
// структура контейнерів для збереження кординат (IMU)
struct IMU_container {
    float x1;
    float y1;
    float z1;
};

struct Data {
    vector<GPS_container> gps;
    vector<IMU_container> imu;
};

#pragma pack(pop);

BIN_READER::BIN_READER(const string& filename) {
    file.open(filename, ios::binary);
}

bool BIN_READER::isOpen() {
    return file.is_open();
}

bool BIN_READER::hasData() {
    return file && !file.eof();
}

void BIN_READER::readBytes(char* buffer, size_t size) {
    file.read(buffer, size);
}

    void parser_for_BIN_class::parse (BIN_READER& reader,Data& storage) {
       
}
