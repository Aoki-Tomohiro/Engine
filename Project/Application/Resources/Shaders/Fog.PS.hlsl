#include "Fog.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Fog
{
    float32_t4x4 projectionInverse;
    float32_t scale;
    float32_t attenuationRate;
};

Texture2D<float32_t> gLinearDepthTexture : register(t0);
Texture2D<float32_t4> gTexture : register(t1);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);
ConstantBuffer<Fog> gFogParameter : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    float32_t ndcDepth = gLinearDepthTexture.Sample(gSamplerPoint, input.texcoord);
    float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gFogParameter.projectionInverse);
    float32_t viewZ = viewSpace.z * rcp(viewSpace.w); //同時座標系からデカルト座標系へ変換
    
    float fogFactor = 1.0f;
    float fogWeight = 0.0f;
    fogWeight += gFogParameter.scale * max(0.0f, 1.0f - exp(-gFogParameter.attenuationRate * viewZ));
    const float3 gbColor = textureColor.rgb;
    const float3 fogColor = 0.8f;
    output.color.rgb = lerp(gbColor, fogColor, fogWeight);
    output.color.a = textureColor.a;
    
    return output;
}