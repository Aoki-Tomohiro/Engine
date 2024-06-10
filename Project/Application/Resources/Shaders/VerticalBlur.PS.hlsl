#include "VerticalBlur.hlsli"

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
    output.color = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
    float32_t4 color = gTexture.Sample(gSampler, input.texcoord);
    float w, h, level;
    //画像の大きさを取得
    gTexture.GetDimensions(0, w, h, level);
    float dx = 1.0f / w;
    float dy = 1.0f / h;

    output.color += gBlur.bkWeight[0] * color;
    for (int i = 1; i <= 7; ++i)
    {
        float2 offset = float2(0, dy * i);
        float2 texcoordPlus = input.texcoord + offset;
        float2 texcoordMinus = input.texcoord - offset;

        if (texcoordPlus.y >= 0.0f && texcoordPlus.y <= 1.0f)
        {
            output.color += gBlur.bkWeight[(i - 1) / 4][(i - 1) % 4] * gTexture.Sample(gSampler, texcoordPlus);
        }

        if (texcoordMinus.y >= 0.0f && texcoordMinus.y <= 1.0f)
        {
            output.color += gBlur.bkWeight[(i - 1) / 4][(i - 1) % 4] * gTexture.Sample(gSampler, texcoordMinus);
        }
    }
    output.color.a = color.a;
    
    return output;
}