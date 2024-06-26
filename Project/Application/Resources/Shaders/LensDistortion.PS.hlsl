#include "LensDistortion.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct LensDistortion
{
    float tightness;
    float strength;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<LensDistortion> gLensDistortionParameter : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    const float2 uvNormalized = input.texcoord * 2 - 1;
    const float distortionMagnitude = abs(uvNormalized.x * uvNormalized.y);
    const float smoothDistortionMagnitude = pow(distortionMagnitude, gLensDistortionParameter.tightness);
	//const float smoothDistortionMagnitude = 1 - sqrt(1 - pow(distortionMagnitude, gLensDistortionParameter.tightness));
	//const float smoothDistortionMagnitude = pow(sin(1.57079632679f * distortionMagnitude), gLensDistortionParameter.tightness);
    float2 uvDistorted = input.texcoord + uvNormalized * smoothDistortionMagnitude * gLensDistortionParameter.strength;
    if (uvDistorted[0] < 0 || uvDistorted[0] > 1 || uvDistorted[1] < 0 || uvDistorted[1] > 1)
    {
        output.color = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
    }
    else
    {
        output.color = gTexture.Sample(gSampler, uvDistorted);
    }
    
    return output;
}