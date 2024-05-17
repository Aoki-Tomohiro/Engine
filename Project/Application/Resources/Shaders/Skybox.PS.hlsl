#include "Skybox.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Material
{
    float32_t4 color;
    float32_t4x4 uvTransform;
};

TextureCube<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    output.color = textureColor * gMaterial.color;
    return output;
}