#include "Outline.hlsli"

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

struct Outline
{
    float32_t4x4 projectionInverse;
    float32_t coefficient;
};

Texture2D<float32_t> gLinearDepthTexture : register(t0);
Texture2D<float32_t4> gTexture : register(t1);
SamplerState gSampler : register(s0);
SamplerState gSamplerPoint : register(s1);
ConstantBuffer<Outline> gOutlineParameter : register(b0);

//インデックス
static const float32_t2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f }, },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f }, },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }, },
};

static const float32_t kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
};

static const float32_t kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f },
};

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    uint32_t width, height; //uvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float32_t2 uvStepSize = float32_t2(rcp(width), rcp(height));
    float32_t2 difference = float32_t2(0.0f, 0.0f); //縦横それぞれの畳み込みの結果を格納する
    for (int x = 0; x < 3; ++x)
    {
        for (int y = 0; y < 3; ++y)
        {
            float32_t2 texcoord = input.texcoord + kIndex3x3[x][y] * uvStepSize;
            float32_t ndcDepth = gLinearDepthTexture.Sample(gSamplerPoint, texcoord);
            float32_t4 viewSpace = mul(float32_t4(0.0f, 0.0f, ndcDepth, 1.0f), gOutlineParameter.projectionInverse);
            float32_t viewZ = viewSpace.z * rcp(viewSpace.w); //同時座標系からデカルト座標系へ変換
            difference.x += viewZ * kPrewittHorizontalKernel[x][y];
            difference.y += viewZ * kPrewittVerticalKernel[x][y];
        }
    }
    //変化の長さをウェイトとして合成。ウェイトの決定方法も色々と考えられる。例えばdifference.xだけ使えば横方向のエッジが検出される
    float32_t weight = length(difference);
    weight = saturate(weight * gOutlineParameter.coefficient); //0~1にしておく
    output.color.rgb = (1.0f - weight) * gTexture.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;
    
    return output;
}