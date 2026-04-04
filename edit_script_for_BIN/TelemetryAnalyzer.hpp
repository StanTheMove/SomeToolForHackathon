#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <limits>
#include <cmath>
#include "Data.hpp"

//Вхідні дані для GPS та IMU

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
        constexpr static double EarthRadius = 6371000.0; 
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
            
            double a = std::pow(std::sin(dLat/2), 2.0) + std::cos(radLat1) * std::cos(radLat2) * std::pow(std::sin(dLon/2), 2.0);
            a = std::min(1.0, std::max(0.0, a));
            double d = 2 * EarthRadius * std::atan2(std::sqrt(a), std::sqrt(1-a));
            
            return d;
        }

        inline double calculateTrapecia(double valPrev, double valCurr, double deltaTime) const {
            return (valPrev + valCurr) / 2.0 * deltaTime;
        }

        inline double velocity(double v_x, double v_y, double v_z) const {
            return std::sqrt(v_x * v_x + v_y * v_y + v_z * v_z);
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

            double velX = 0.0, velY = 0.0, velZ = 0.0;
            for(size_t i = 1; i < imuData.size(); ++i)
            {
                double dt = (imuData[i].timestamp - imuData[i-1].timestamp); // перетворення мілісекунд в секунди

                if(dt <= 0) continue;

                double dvx = calculateTrapecia(imuData[i-1].accelX, imuData[i].accelX, dt);
                double dvy = calculateTrapecia(imuData[i-1].accelY, imuData[i].accelY, dt);
                double dvz = calculateTrapecia(imuData[i-1].accelZ - 9.81, imuData[i].accelZ - 9.81, dt);

                velX += dvx; velY += dvy; velZ += dvz;

                double currentSpeed = velocity(velX, velY, velZ);
                if (currentSpeed > metrics.maxSpeed)
                {
                    metrics.maxSpeed = currentSpeed;
                }

                double currentAccel = velocity(imuData[i].accelX, imuData[i].accelY, imuData[i].accelZ - 9.81);
                if (currentAccel > metrics.maxAcceleration)
                {
                    metrics.maxAcceleration = currentAccel;
                }
            }
            return metrics;
        }

};