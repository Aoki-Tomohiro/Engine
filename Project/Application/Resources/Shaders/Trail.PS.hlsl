#include "Trail.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Material
{
    float32_t4 startColor;
    float32_t4 endColor;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t4 startColor = textureColor * gMaterial.startColor;
    float32_t4 endColor = textureColor * gMaterial.endColor;
    output.color.rgb = lerp(startColor, endColor, input.texcoord.x);
    output.color.a = lerp(gMaterial.startColor.w, gMaterial.endColor.w, input.texcoord.x);
    return output;
}