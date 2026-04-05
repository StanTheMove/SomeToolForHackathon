#define NOMINMAX
#include <iostream>
#include <windows.h>
#include "BIN_class_struct.hpp"
#include "parser_for_BIN_class.hpp"
#include "Data.hpp"
#include "TelemetryAnalyzer.hpp"
#include <fstream>
using namespace std;

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);

    BIN_READER reader_gps("gps.bin");  
    if (!reader_gps.isOpen()) {
        cout << "do not open gps file\n";
        return 1;
    }

    BIN_READER reader_imu("imu.bin"); 
    if (!reader_imu.isOpen()) {
        cout << "do not open imu file\n";
        return 1;
    }

    Data storage;
    Parser_for_BIN_class::parse(reader_gps, storage);

    cout << "GPS points: " << storage.gps.size() << endl;
    cout << "IMU records: " << storage.imu.size() << endl;

    for (const auto& g : storage.gps) {
        cout << "GPS: " << g.altitude << " " << g.latitude << " " << g.longitude << endl;
    }
    for (const auto& i : storage.imu) {
        cout << "IMU: " << i.accelX << " " << i.accelY << " " << i.accelZ << endl;
    }

    TelemetryAnalyzer analyzer;
    FlightMetrics metrics = analyzer.analyzeTelemetry(storage.gps, storage.imu);
    cout << "Total Distance:   " << metrics.totalDistance << " m" << endl;
    cout << "Flight Duration:  " << metrics.flightDuration << " s" << endl;
    cout << "Max Speed:        " << metrics.maxSpeed << " m/s" << endl;
    cout << "Max Acceleration: " << metrics.maxAcceleration << " m/s^2" << endl;

    // Записуємо метрики І GPS точки у JSON
    ofstream json_file("metrics.json");
    json_file << "{\n";
    json_file << "  \"total_distance\": " << metrics.totalDistance << ",\n";
    json_file << "  \"flight_duration\": " << metrics.flightDuration << ",\n";
    json_file << "  \"max_speed\": " << metrics.maxSpeed << ",\n";
    json_file << "  \"max_acceleration\": " << metrics.maxAcceleration << ",\n";
    json_file << "  \"gps_points\": [\n";
    for (size_t i = 0; i < storage.gps.size(); i++) {
        json_file << "    {";
        json_file << "\"lat\": " << storage.gps[i].latitude << ", ";
        json_file << "\"lon\": " << storage.gps[i].longitude << ", ";
        json_file << "\"alt\": " << storage.gps[i].altitude;
        json_file << "}";
        if (i < storage.gps.size() - 1) json_file << ",";
        json_file << "\n";
    }
    json_file << "  ]\n";
    json_file << "}\n";
    json_file.close();

    return 0;
}