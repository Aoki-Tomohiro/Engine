#include "Color.h"
#include <cmath>

Color Color::ToSRGB() const
{
    Color resultColor{};
    resultColor.value_.x = SRGBComponent(value_.x);
    resultColor.value_.y = SRGBComponent(value_.y);
    resultColor.value_.z = SRGBComponent(value_.z);
    resultColor.value_.w = value_.w;
    return resultColor;
}

Color Color::FromSRGB() const
{
    Color resultColor{};
    resultColor.value_.x = InverseSRGBComponent(value_.x);
    resultColor.value_.y = InverseSRGBComponent(value_.y);
    resultColor.value_.z = InverseSRGBComponent(value_.z);
    resultColor.value_.w = value_.w;
    return resultColor;
}

float Color::SRGBComponent(float value)
{
    float result = std::pow(value, 1.0f / 2.4f);
    result = result * 1.055f - 0.055f;
    result = (value <= 0.0031308f) ? value * 12.92f : result;

    return std::min(std::max(result, 0.0f), 1.0f);
}

float Color::InverseSRGBComponent(float value)
{
    float result = (value <= 0.04045f) ? value / 12.92f : std::pow((value + 0.055f) / 1.055f, 2.4f);
    return std::min(std::max(result, 0.0f), 1.0f);
}