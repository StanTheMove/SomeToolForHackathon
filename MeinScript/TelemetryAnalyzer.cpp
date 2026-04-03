#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>

//Вхідні дані для GPS та IMU
struct GPSdata
{
    double timestamp;
    double latitude;
    double longitude;
    double altitude; 
};

struct IMUdata
{
    double timestamp;
    double TaccelX;
    double TaccelY;
    double TaccelZ;
};

struct FlightMetrics
{
    double totalDistance = 0.0;
    double maxSpeed = 0.0;
    double maxAcceleration = 0.0;
    double flightDuration = 0.0;
};

class TelemetryAnalyzer
{
    private:
        constexpr static double EarthRadius = 6371000.0; // радіус Землі в метрах
        constexpr static double PI = 3.14159265358979323846; 

        inline double convertToRadians(double degree) const{ 
            return degree * PI / 180.0;
        }

        //метод для обчислення дистанції через гаверсинус
        double calculateHaversine(double lat1, double lon1, double lat2, double lon2) const
        {
            double dLat = convertToRadians(lat1 - lat2);
            double dLon = convertToRadians(lon1 - lon2);
            double radLat1 = convertToRadians(lat1);
            double radLat2 = convertToRadians(lat2);

            double a = std::pow(std::sin(dLat/2), 2) + std::cos(radLat1) * std::cos(radLat2) * std::pow(std::sin(dLon/2), 2);
            double d = 2 * EarthRadius * std::atan2(std::sqrt(a), std::sqrt(1-a));
            
            a = std::min(1.0, std::max(0.0, a)); // обмеження a до 1 для уникнення помилок через округлення

            return d;
        }

    public:
        FlightMetrics analyzeTelemetry(const std::vector<GPSdata>& gpsData, const std::vector<IMUdata>& imuData)
        {
            FlightMetrics metrics;

            if(gpsData.empty() || imuData.empty())
            {
                return metrics;
            }

            metrics.flightDuration = gpsData.back().timestamp - gpsData.front().timestamp;

            for(size_t i = 1; i < gpsData.size(); ++i)
            {
                double deltaDistance = calculateHaversine(gpsData[i - 1].latitude, gpsData[i-1].longitude, gpsData[i].latitude, gpsData[i].longitude);
                metrics.totalDistance += deltaDistance;
            }

            return metrics;
        }
         
};