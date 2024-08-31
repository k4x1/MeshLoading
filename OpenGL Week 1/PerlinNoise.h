#pragma once
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>

class PerlinNoise {
public:
    PerlinNoise() {
        Seed = static_cast<int>(time(nullptr));
    }

    double RandomValue(int X, int Y) {
        int Noise = X + Y * Seed;
        Noise = (Noise << 13) ^ Noise;
        int T = (Noise * (Noise * Noise * 15731 + 789221) + 1376312589) & 0x7fffffff;
        return 1.0 - (double)T * 0.93132257461548515625e-9;
    }

    double Smooth(int X, int Y) {
        double Corners = (RandomValue(X - 1, Y - 1) + RandomValue(X + 1, Y - 1) +
            RandomValue(X - 1, Y + 1) + RandomValue(X + 1, Y + 1)) / 16.0;
        double Sides = (RandomValue(X - 1, Y) + RandomValue(X + 1, Y) +
            RandomValue(X, Y - 1) + RandomValue(X, Y + 1)) / 8.0;
        double Center = RandomValue(X, Y) / 4.0;
        return Corners + Sides + Center;
    }

    double CosineInterpolate(double Point1, double Point2, double Fract) {
        double Fract2 = (1 - cos(Fract * 3.14159265359)) / 2;
        return Point1 * (1 - Fract2) + Point2 * Fract2;
    }

    double SmoothInterpolate(double X, double Y) {
        int TruncatedX = (int)X;
        int TruncatedY = (int)Y;
        double FractX = X - (double)TruncatedX;
        double FractY = Y - (double)TruncatedY;

        double V1 = Smooth(TruncatedX, TruncatedY);
        double V2 = Smooth(TruncatedX + 1, TruncatedY);
        double V3 = Smooth(TruncatedX, TruncatedY + 1);
        double V4 = Smooth(TruncatedX + 1, TruncatedY + 1);

        double Interpolate_1 = CosineInterpolate(V1, V2, FractX);
        double Interpolate_2 = CosineInterpolate(V3, V4, FractX);

        return CosineInterpolate(Interpolate_1, Interpolate_2, FractY);
    }

    double TotalNoisePerPoint(int X, int Y, int Octaves = 6, float Wavelength = 128.0f, float Gain = 0.5f, float Lacunarity = 2.0f) {
        float MaxValue = 0.0f;
        double Total = 0.0f;

        for (int i = 0; i < Octaves; i++) {
            float Frequency = pow(Lacunarity, i) / Wavelength;
            float Amplitude = pow(Gain, i);
            MaxValue += Amplitude;

            Total += SmoothInterpolate(X * Frequency, Y * Frequency) * Amplitude;
        }

        return Total / MaxValue;
    }

private:
    int Seed;
};
