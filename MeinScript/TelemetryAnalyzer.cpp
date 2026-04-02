#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

//Вхідні дані для GPS та IMU
struct GPSdata
{
    float timestamp;
    float Tlatitude;
    float Tlongitude;
    float Taltitude; 
};

struct IMUdata
{
    float timestamp;
    float TaccelX;
    float TaccelY;
    float TaccelZ;
};

struct FlightMetrics
{
    float totalDistance = 0.0f;
    float maxSpeed = 0.0f;
    float maxAcceleration = 0.0f;
    float flightDuration = 0.0f;
};

class TelemetryAnalyzer
{
    constexpr static float EarthRadius = 6371000.0f; 
    
    public:
        FlightMetrics analyzeTelemetry(const std::vector<GPSdata>& gpsData, const std::vector<IMUdata>& imuData)
        {
            FlightMetrics metrics;

            if(gpsData.empty() || imuData.empty())
            {
                return metrics;
            }
            return metrics;
        }
         
};