#include "Object3d.hlsli"

struct WorldTransform
{
    float32_t4x4 world;
    float32_t4x4 worldInverseTranspose;
};

struct Camera
{
    float32_t3 worldPosition;
    float32_t4x4 view;
    float32_t4x4 projection;
};

ConstantBuffer<WorldTransform> gWorldTransform : register(b0);
ConstantBuffer<Camera> gCamera : register(b1);
ConstantBuffer<Camera> gLightCamera : register(b2);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(input.position, mul(gWorldTransform.world, mul(gCamera.view, gCamera.projection)));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) gWorldTransform.worldInverseTranspose));
    output.worldPosition = mul(input.position, gWorldTransform.world).xyz;
    output.toEye = normalize(gCamera.worldPosition - output.worldPosition);
    output.cameraToPosition = normalize(output.worldPosition - gCamera.worldPosition);
    
    
    float32_t4x4 lightWorldViewProjection = mul(gWorldTransform.world, mul(gLightCamera.view, gLightCamera.projection));
    float32_t4 lightToPosition = mul(input.position, lightWorldViewProjection);
    lightToPosition.xyz /= lightToPosition.w;
    output.positionSM.x = (lightToPosition.x * 0.5f) + 0.5f;
    output.positionSM.y = (-lightToPosition.y * 0.5f) + 0.5f;
    output.positionSM.z = lightToPosition.z;

    return output;
}