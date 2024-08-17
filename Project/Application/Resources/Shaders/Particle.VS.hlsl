#include "Particle.hlsli"

struct PerView
{
    float32_t4x4 viewMatrix;
    float32_t4x4 projectionMatrix;
    float32_t4x4 billboardMatrix;
};

StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PerView> gPerView : register(b1);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

float32_t4x4 MakeRotateXMatrix(float radian)
{
    float32_t4x4 result;
    result[0][0] = 1;
    result[0][1] = 0;
    result[0][2] = 0;
    result[0][3] = 0;

    result[1][0] = 0;
    result[1][1] = cos(radian);
    result[1][2] = sin(radian);
    result[1][3] = 0;

    result[2][0] = 0;
    result[2][1] = -sin(radian);
    result[2][2] = cos(radian);
    result[2][3] = 0;

    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;

    return result;
}


float32_t4x4 MakeRotateYMatrix(float radian)
{
    float32_t4x4 result;
    result[0][0] = cos(radian);
    result[0][1] = 0;
    result[0][2] = -sin(radian);
    result[0][3] = 0;

    result[1][0] = 0;
    result[1][1] = 1;
    result[1][2] = 0;
    result[1][3] = 0;

    result[2][0] = sin(radian);
    result[2][1] = 0;
    result[2][2] = cos(radian);
    result[2][3] = 0;

    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;

    return result;
}


float32_t4x4 MakeRotateZMatrix(float radian)
{
    float32_t4x4 result;
    result[0][0] = cos(radian);
    result[0][1] = sin(radian);
    result[0][2] = 0;
    result[0][3] = 0;

    result[1][0] = -sin(radian);
    result[1][1] = cos(radian);
    result[1][2] = 0;
    result[1][3] = 0;

    result[2][0] = 0;
    result[2][1] = 0;
    result[2][2] = 1;
    result[2][3] = 0;

    result[3][0] = 0;
    result[3][1] = 0;
    result[3][2] = 0;
    result[3][3] = 1;

    return result;
}

float32_t4x4 MakeIdentity4x4()
{
    float32_t4x4 result;
    result[0][0] = 1.0f;
    result[0][1] = 0.0f;
    result[0][2] = 0.0f;
    result[0][3] = 0.0f;

    result[1][0] = 0.0f;
    result[1][1] = 1.0f;
    result[1][2] = 0.0f;
    result[1][3] = 0.0f;

    result[2][0] = 0.0f;
    result[2][1] = 0.0f;
    result[2][2] = 1.0f;
    result[2][3] = 0.0f;

    result[3][0] = 0.0f;
    result[3][1] = 0.0f;
    result[3][2] = 0.0f;
    result[3][3] = 1.0f;

    return result;
}

bool AreMatricesEqual(float4x4 matrixA, float4x4 matrixB)
{
    for (int i = 0; i < 4; i++)
    {
        if (any(matrixA[i] != matrixB[i]))
        {
            return false;
        }
    }
    return true;
}

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    VertexShaderOutput output;
    Particle particle = gParticle[instanceId];
    float32_t4x4 worldMatrix = MakeIdentity4x4();
    float32_t4x4 rotateXMatrix = MakeRotateXMatrix(particle.rotate.x);
    float32_t4x4 rotateYMatrix = MakeRotateYMatrix(particle.rotate.y);
    float32_t4x4 rotateZMatrix = MakeRotateZMatrix(particle.rotate.z);
    float32_t4x4 rotateMatrix = mul(rotateXMatrix, mul(rotateYMatrix, rotateZMatrix));
    if (AreMatricesEqual(worldMatrix, gPerView.billboardMatrix))
    {
        worldMatrix = rotateMatrix;
    }
    else
    {
        worldMatrix = mul(gPerView.billboardMatrix, rotateZMatrix);
    }
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    output.position = mul(input.position, mul(worldMatrix, mul(gPerView.viewMatrix, gPerView.projectionMatrix)));
    output.texcoord = input.texcoord;
    output.color = particle.color;

    return output;
}