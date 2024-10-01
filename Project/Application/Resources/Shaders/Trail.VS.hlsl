#include "Trail.hlsli"

struct Camera
{
    float32_t3 worldPosition;
    float32_t4x4 view;
    float32_t4x4 projection;
};

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
};

ConstantBuffer<Camera> gCamera : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, mul(gCamera.view, gCamera.projection));
    output.texcoord = input.texcoord;
    return output;
}