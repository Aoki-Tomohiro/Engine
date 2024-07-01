#include "RadialBlur.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct RadialBlur
{
    float32_t2 center;
    float32_t blurWidth;
};

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<RadialBlur> gRadialBlurParameter : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    const int32_t kNumSamplers = 10; //サンプリング数。多いほど滑らかだが重い
    float32_t2 direction = input.texcoord - gRadialBlurParameter.center;
    float32_t3 outputColor = float32_t3(0.0f, 0.0f, 0.0f);
    for (int32_t sampleIndex = 0; sampleIndex < kNumSamplers; ++sampleIndex)
    {
        //現在のuvから先ほど計算した方向にサンプリング点を進めながらサンプリングしていく
        float32_t2 texcoord = input.texcoord + direction * gRadialBlurParameter.blurWidth * float32_t(sampleIndex);
        outputColor.rgb += gTexture.Sample(gSampler, texcoord).rgb;
    }
    //平均化する
    outputColor.rgb *= rcp(kNumSamplers);
    output.color.rgb = outputColor;
    output.color.a = 1.0f;
    return output;
}