#include "PostEffects.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct LensDistortion
{
    int32_t isEnable;
    float tightness;
    float strength;
};

struct Vignette
{
    int32_t isEnable;
    float scale;
    float intensity;
};

struct GrayScale
{
    int32_t isEnable;
    int32_t isSepiaEnable;
};

struct BoxFilter
{
    int32_t isEnable;
    int32_t kernelSize;
};

Texture2D<float32_t4> gTexture : register(t0);
Texture2D<float32_t4> gRenderedEffectsTexture : register(t1);
SamplerState gSampler : register(s0);

ConstantBuffer<LensDistortion> gLensDistortionParameter : register(b0);
ConstantBuffer<Vignette> gVignetteParameter : register(b1);
ConstantBuffer<GrayScale> gGrayScaleParameter : register(b2);
ConstantBuffer<BoxFilter> gBoxFilterParameter : register(b3);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    //通常テクスチャ
    float32_t4 originalColor = gTexture.Sample(gSampler, input.texcoord);
    //ポストエフェクトをかけたテクスチャ
    float32_t4 effectsColor = gRenderedEffectsTexture.Sample(gSampler, input.texcoord);
    //ポストエフェクトをかけたテクスチャが真っ黒の場合はoriginalColorを使う
    if (effectsColor.r == 0 && effectsColor.g == 0 && effectsColor.b == 0 && effectsColor.a == 0)
    {
        output.color = originalColor;
    }
    else
    {
        output.color = effectsColor;

    }
    
    //レンズディストーション
    if (gLensDistortionParameter.isEnable)
    {
        const float2 uvNormalized = input.texcoord * 2 - 1;
        const float distortionMagnitude = abs(uvNormalized.x * uvNormalized.y);
        const float smoothDistortionMagnitude = pow(distortionMagnitude, gLensDistortionParameter.tightness);
		//const float smoothDistortionMagnitude = 1 - sqrt(1 - pow(distortionMagnitude, gLensDistortionParameter.tightness));
		//const float smoothDistortionMagnitude = pow(sin(1.57079632679f * distortionMagnitude), gLensDistortionParameter.tightness);
        float2 uvDistorted = input.texcoord + uvNormalized * smoothDistortionMagnitude * gLensDistortionParameter.strength;
        if (uvDistorted[0] < 0 || uvDistorted[0] > 1 || uvDistorted[1] < 0 || uvDistorted[1] > 1)
        {
            originalColor = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
            effectsColor = float32_t4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        else
        {
            originalColor = gTexture.Sample(gSampler, uvDistorted);
            effectsColor = gRenderedEffectsTexture.Sample(gSampler, uvDistorted);
            //ポストエフェクトをかけたテクスチャが真っ黒の場合はoriginalColorを使う
            if (effectsColor.r == 0 && effectsColor.g == 0 && effectsColor.b == 0 && effectsColor.a == 0)
            {
                output.color = originalColor;
            }
            else
            {
                output.color = effectsColor;

            }
        }
    }
    
    //ビネット
    if (gVignetteParameter.isEnable)
    {
        //周囲を0に、中心になるほど明るくなるように計算で調整
        float32_t2 correct = input.texcoord * (1.0f - input.texcoord.yx);
        //correctだけで計算すると中心の最大値が0.0625で暗すぎるのでScaleで調整。この例では16倍して1にしている
        float vignette = correct.x * correct.y * gVignetteParameter.scale;
        //とりあえず0.8乗でそれっぽくしてみた
        vignette = saturate(pow(vignette, gVignetteParameter.intensity));
        //係数として乗算
        output.color.rgb *= vignette;
    }
    
    //GrayScale
    if(gGrayScaleParameter.isEnable)
    {
        float32_t value = dot(output.color.rgb, float32_t3(0.2125f, 0.7154f, 0.0721f));
        if(gGrayScaleParameter.isSepiaEnable)
        {
            output.color.rgb = value * float32_t3(1.0f, 74.0f / 107.0f, 43.0f / 107.0f);
        }
        else
        {
            output.color.rgb = float32_t3(value, value, value);
        }
    }
    
    //BoxFilter
    if (gBoxFilterParameter.isEnable)
    {        
        //インデックス
        static const float32_t2 kIndex3x3[3][3] =
        {
            { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, },
            { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, },
            { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, },
        };
        
        static const float32_t2 kIndex5x5[5][5] =
        {
            { { -2.0f, -2.0f }, { -1.0f, -2.0f }, { 0.0f, -2.0f }, { 1.0f, -2.0f }, { 2.0f, -2.0f }, },
            { { -2.0f, -1.0f }, { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, { 2.0f, -1.0f }, },
            { { -2.0f, 0.0f }, { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 2.0f, 0.0f }, },
            { { -2.0f, 1.0f }, { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, { 2.0f, 1.0f }, },
            { { -2.0f, 2.0f }, { -1.0f, 2.0f }, { 0.0f, 2.0f }, { 1.0f, 2.0f }, { 2.0f, 2.0f }, },
        };

        //カーネル
        static const float32_t kKernel3x3[3][3] =
        {
            { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
            { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
            { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
        };
        
        static const float32_t kKernel5x5[5][5] =
        {
            { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
            { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
            { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
            { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
            { 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f, 1.0f / 25.0f },
        };
        
        uint32_t width, height; //uvStepSizeの算出
        gTexture.GetDimensions(width, height);
        float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
        if(gBoxFilterParameter.kernelSize == 0)
        {
            for (int32_t x = 0; x < 3; ++x)
            {
                for (int32_t y = 0; y < 3; ++y)
                {
                    float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
                    float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                    output.color.rgb += fetchColor * kKernel3x3[x][y];
                }
            }
        }
        else
        {
            for (int32_t x = 0; x < 5; ++x)
            {
                for (int32_t y = 0; y < 5; ++y)
                {
                    float32_t2 texcoord = input.texcoord + kIndex5x5[x][y] * uvStepSize;
                    float32_t3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
                    output.color.rgb += fetchColor * kKernel5x5[x][y];
                }
            }
        }
    }
    
    return output;
}