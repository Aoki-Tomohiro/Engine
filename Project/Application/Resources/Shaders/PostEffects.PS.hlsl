#include "PostEffects.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct HSV
{
    float32_t hue;
    float32_t saturation;
    float32_t value;
};

Texture2D<float32_t4> gTexture : register(t0);
ConstantBuffer<HSV> gHSVParameter : register(b0);
SamplerState gSampler : register(s0);

float32_t3 RGBToHSV(float32_t3 rgb)
{
    float32_t max = rgb.x > rgb.y ? rgb.x : rgb.y;
    max = max > rgb.z ? max : rgb.z;
    float32_t min = rgb.x < rgb.y ? rgb.x : rgb.y;
    min = min < rgb.z ? min : rgb.z;
    float32_t h = max - min;
    if(h > 0.0f)
    {
        if(max == rgb.x)
        {
            h = (rgb.y - rgb.z) / h;
            if(h < 0.0f)
            {
                h += 6.0f;
            }
        }
        else if(max == rgb.y)
        {
            h = 2.0f + (rgb.z - rgb.x) / h;
        }
        else
        {
            h = 4.0f + (rgb.x - rgb.y) / h;
        }
    }
    h /= 6.0f;
    float32_t s = (max - min);
    if(max != 0.0f)
    {
        s /= max;
    }
    float32_t v = max;
    return float32_t3(h, s, v);
}

float32_t3 HSVToRGB(float32_t3 hsv)
{
    float32_t r = hsv.z;
    float32_t g = hsv.z;
    float32_t b = hsv.z;
    if(hsv.y > 0.0f)
    {
        hsv.x *= 6.0f;
        int32_t i = int32_t(hsv.x);
        float32_t f = hsv.x - float32_t(i);
        if(i == 0)
        {
            g *= 1.0f - hsv.y * (1.0f - f);
            b *= 1.0f - hsv.y;
        }
        else if(i == 1)
        {
            r *= 1.0f - hsv.y * f;
            b *= 1.0f - hsv.y;
        }
        else if(i == 2)
        {
            r *= 1.0f - hsv.y;
            b *= 1.0f - hsv.y * (1.0f - f);
        }
        else if(i == 3)
        {
            r *= 1.0f - hsv.y;
            g *= 1.0f - hsv.y * f;
        }
        else if(i == 4)
        {
            r *= 1.0f - hsv.y * (1.0f - f);
            g *= 1.0f - hsv.y;
        }
        else if(i == 5)
        {
            g *= 1.0f - hsv.y;
            b *= 1.0f - hsv.y * f;
        }
    }
    return float32_t3(r, g, b);
}

float32_t WrapValue(float32_t value, float32_t minRange, float32_t maxRange)
{
    float32_t range = maxRange - minRange;
    float32_t modValue = fmod(value - minRange, range);
    if(modValue < 0)
    {
        modValue += range;
    }
    return minRange + modValue;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t3 hsv = RGBToHSV(textureColor.rgb);
    hsv.x += gHSVParameter.hue;
    hsv.y += gHSVParameter.saturation;
    hsv.z += gHSVParameter.value;
    hsv.x = WrapValue(hsv.x, 0.0f, 1.0f);
    hsv.y = saturate(hsv.y);
    hsv.z = saturate(hsv.z);
    float32_t3 rgb = HSVToRGB(hsv);
    output.color.rgb = rgb;
    output.color.a = textureColor.a;
    return output;
}