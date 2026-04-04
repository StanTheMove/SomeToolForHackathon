#define NOMINMAX
#include <iostream>
#include <windows.h>
#include "BIN_class_struct.hpp"
#include "parser_for_BIN_class.hpp"
#include "Data.hpp"
#include "TelemetryAnalyzer.hpp"

using namespace std;
// Головна функція реалізації проекту
int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    BIN_READER reader("log.bin"); 

    if (!reader.isOpen()) {
        cout << "do not open file\n";
        return 1;
    }

    Data storage;

    Parser_for_BIN_class::parse(reader, storage);

    cout << "GPS points: " << storage.gps.size() << endl;
    cout << "IMU records: " << storage.imu.size() << endl;

    // for (const auto& g : storage.gps) {
    //     cout << "GPS: " << g.altitude << " " << g.latitude << " " << g.longitude << endl;
    // }

    // for (const auto& i : storage.imu) {
    //     cout << "IMU: " << i.accelX << " " << i.accelY << " " << i.accelZ << endl;
    // }

    TelemetryAnalyzer analyzer;
    FlightMetrics metrics = analyzer.analyzeTelemetry(storage.gps, storage.imu);
    cout << "Total Distance:   " << metrics.totalDistance << " m" << endl;
    cout << "Flight Duration:  " << metrics.flightDuration << " s" << endl;
    cout << "Max Speed:        " << metrics.maxSpeed << " m/s" << endl;
    cout << "Max Acceleration: " << metrics.maxAcceleration << " m/s^2" << endl;
    
    return 0;
}