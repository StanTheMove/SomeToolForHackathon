#define NOMINMAX
#include <iostream>
#include <windows.h>
#include "BIN_class_struct.hpp"
#include "parser_for_BIN_class.hpp"
#include "Data.hpp"
#include "TelemetryAnalyzer.hpp"
#include <fstream>

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

    // Аналіз даних та вивід результатів
    TelemetryAnalyzer analyzer;
    FlightMetrics metrics = analyzer.analyzeTelemetry(storage.gps, storage.imu);
    cout << "Total Distance:   " << metrics.totalDistance << " m" << endl;
    cout << "Flight Duration:  " << metrics.flightDuration << " s" << endl;
    cout << "Max Speed:        " << metrics.maxSpeed << " m/s" << endl;
    cout << "Max Acceleration: " << metrics.maxAcceleration << " m/s^2" << endl;

    //код для запису результатів у JSON файл
    ofstream json_file("metrics.json");
    json_file << "{\n";
    json_file << "  \"total_distance\": " << metrics.totalDistance << ",\n";
    json_file << "  \"flight_duration\": " << metrics.flightDuration << ",\n";
    json_file << "  \"max_speed\": " << metrics.maxSpeed << ",\n";
    json_file << "  \"max_acceleration\": " << metrics.maxAcceleration << "\n";
    json_file << "}\n";
    json_file.close();
    return 0;
}