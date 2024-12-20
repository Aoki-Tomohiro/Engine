#include "DepthOfField.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct DepthOfField
{
    float32_t4x4 projectionInverse;
    float32_t focusDepth;
    float32_t nFocusWidth;
    float32_t fFocusWidth;
};

Texture2D<float32_t> gLinearDepthTexture : register(t0);
Texture2D<float32_t4> gTexture : register(t1);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);

ConstantBuffer<DepthOfField> gDepthOfFieldParameter : register(b0);

float32_t Gaussian(float32_t2 drawUV, float32_t2 pickUV, float32_t sigma)
{
    float32_t d = distance(drawUV, pickUV);
    return exp(-(d * d) / (2 * sigma * sigma));
}

float32_t4 GaussianBlur(float32_t2 uv, float32_t sigma, float32_t stepWidth)
{
    float32_t totalWeight = 0.0f;
    float32_t4 color = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
    
    for (float py = -sigma * 2; py <= sigma * 2; py += stepWidth)
    {
        for (float px = -sigma * 2; px <= sigma * 2; px += stepWidth)
        {
            float32_t2 pickUV = uv + float32_t2(px, py);
            float32_t ndcDepth = gLinearDepthTexture.Sample(gSamplerPoint, pickUV);
            float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gDepthOfFieldParameter.projectionInverse);
            float32_t pickDepth = viewSpace.z * rcp(viewSpace.w);
            float32_t pickFocus = smoothstep(0, gDepthOfFieldParameter.nFocusWidth, abs(pickDepth - gDepthOfFieldParameter.focusDepth));
            float32_t weight = Gaussian(uv, pickUV, sigma) * pickFocus;
            color += gTexture.Sample(gSampler, pickUV) * weight;
            totalWeight += weight;
        }
    }
    color.rgb = color.rgb / totalWeight;
    return color;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    float32_t ndcDepth = gLinearDepthTexture.Sample(gSampler, input.texcoord);
    float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gDepthOfFieldParameter.projectionInverse);
    float32_t viewZ = viewSpace.z * rcp(viewSpace.w); //同時座標系からデカルト座標系へ変換
    float32_t4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    float inFocus = 1 - smoothstep(0, gDepthOfFieldParameter.nFocusWidth, abs(viewZ - gDepthOfFieldParameter.focusDepth));
    float outFocus = smoothstep(gDepthOfFieldParameter.nFocusWidth, gDepthOfFieldParameter.fFocusWidth, abs(viewZ - gDepthOfFieldParameter.focusDepth));
    float middleFocus = 1 - inFocus - outFocus;
    
    float32_t4 inFocusColor = textureColor;
    float32_t4 middleFocusColor = GaussianBlur(input.texcoord, 0.001f, 0.0002f);
    float32_t4 outFocusColor = GaussianBlur(input.texcoord, 0.0016f, 0.0004f);
    
    output.color = inFocus * inFocusColor + middleFocus * middleFocusColor + outFocus * outFocusColor;
 
    return output;
}