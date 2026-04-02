#include <iostream>
#include <windows.h>
#include "BIN_class_struct.hpp"
#include "parser_for_BIN_class.hpp"
#include "Data.hpp"

using namespace std;
// Головна функція реалізації проекту
int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    BIN_READER reader("log.bin"); 

    if (!reader.isOpen()) {
        cout << "Не вдалося відкрити файл\n";
        return 1;
    }

    Data storage;

    Parser_for_BIN_class::parse(reader, storage);

    cout << "GPS points: " << storage.gps.size() << endl;
    cout << "IMU records: " << storage.imu.size() << endl;

    for (const auto& g : storage.gps) {
        cout << "GPS: " << g.x << " " << g.y << endl;
    }

    for (const auto& i : storage.imu) {
        cout << "IMU: " << i.x1 << " " << i.y1 << " " << i.z1 << endl;
    }

    return 0;
}