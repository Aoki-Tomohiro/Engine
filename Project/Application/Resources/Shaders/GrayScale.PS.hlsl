#include "GrayScale.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct GrayScale
{
    int32_t isSepiaEnable;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<GrayScale> gGrayScaleParameter : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t value = dot(textureColor.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
    if (gGrayScaleParameter.isSepiaEnable)
    {
        output.color.rgb = value * float32_t3(1.0f, 74.0f / 107.0f, 43.0f / 107.0f);
    }
    else
    {
        output.color.rgb = float32_t3(value, value, value);
    }
    output.color.a = 1.0f;
    
    return output;
}