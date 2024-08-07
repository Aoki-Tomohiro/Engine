struct WorldTransform
{
    float32_t4x4 world;
    float32_t4x4 worldInverseTranspose;
};

struct LightCamera
{
    float32_t3 worldPosition;
    float32_t4x4 view;
    float32_t4x4 projection;
};

ConstantBuffer<WorldTransform> gWorldTransform : register(b0);
ConstantBuffer<LightCamera> gLightCamera : register(b1);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
};

struct VertexShaderOutput
{
    float32_t4 position : SV_POSITION;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    float32_t4x4 worldViewProjection = mul(gWorldTransform.world, mul(gLightCamera.view, gLightCamera.projection));
    output.position = mul(input.position, worldViewProjection);
    return output;
}