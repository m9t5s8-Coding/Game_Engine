#include <Angle.hpp>

#define _USE_MATH_DEFINES

#include <cmath>

float aero::degrees(const float radians)
{
    return radians * 180 / (float)M_PI;
}

float aero::radians(const float degrees)
{
    return degrees * (float)M_PI / 180;
}
