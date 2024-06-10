#include "HorizontalBlur.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Blur
{
    int32_t textureWidth;
    int32_t textureHeight;
    float4 bkWeight[2];
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

ConstantBuffer<Blur> gBlur : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    output.color = float32_t4(0, 0, 0, 0);
    float32_t4 color = gTexture.Sample(gSampler, input.texcoord);
    float w = gBlur.textureWidth;
    float h = gBlur.textureHeight;
    float dx = 1.0f / w;
    float dy = 1.0f / h;

    output.color += gBlur.bkWeight[0] * color;
    for (int i = 1; i <= 7; ++i)
    {
        float2 offset = float2(dx * i, 0);
        float2 texcoordPlus = input.texcoord + offset;
        float2 texcoordMinus = input.texcoord - offset;

        if (texcoordPlus.x >= 0.0f && texcoordPlus.x <= 1.0f)
        {
            output.color += gBlur.bkWeight[(i - 1) / 4][(i - 1) % 4] * gTexture.Sample(gSampler, texcoordPlus);
        }

        if (texcoordMinus.x >= 0.0f && texcoordMinus.x <= 1.0f)
        {
            output.color += gBlur.bkWeight[(i - 1) / 4][(i - 1) % 4] * gTexture.Sample(gSampler, texcoordMinus);
        }
    }
    output.color.a = color.a;

    return output;
}