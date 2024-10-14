#include "Particle.hlsli"

struct PerView
{
    float32_t4x4 viewMatrix;
    float32_t4x4 projectionMatrix;
    float32_t4x4 billboardMatrix;
    float32_t3 worldPosition;
};

StructuredBuffer<Particle> gParticle : register(t0);
ConstantBuffer<PerView> gPerView : register(b1);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input, uint32_t instanceId : SV_InstanceID)
{
    //アウトプット
    VertexShaderOutput output;
    //パーティクルを取得
    Particle particle = gParticle[instanceId];
    
    //回転行列の計算
    float32_t4x4 rotateMatrix = MakeIdentity4x4();
    if (AreQuaternionEqual(particle.quaternion, float32_t4(0.0f, 0.0f, 0.0f, 1.0f)))
    {
        float32_t4x4 rotateXMatrix = MakeRotateXMatrix(particle.rotate.x);
        float32_t4x4 rotateYMatrix = MakeRotateYMatrix(particle.rotate.y);
        float32_t4x4 rotateZMatrix = MakeRotateZMatrix(particle.rotate.z);
        rotateMatrix = mul(rotateXMatrix, mul(rotateYMatrix, rotateZMatrix));
    }
    else
    {
        rotateMatrix = MakeRotateMatrix(particle.quaternion);
    }
    
    //ビルボード行列の計算
    float32_t4x4 worldMatrix = (AreMatricesEqual(gPerView.billboardMatrix, MakeIdentity4x4()) || particle.alignToDirection) ?
    rotateMatrix : mul(MakeRotateZMatrix(particle.rotate.z), gPerView.billboardMatrix);
    
    //スケールの適用
    worldMatrix[0] *= particle.scale.x;
    worldMatrix[1] *= particle.scale.y;
    worldMatrix[2] *= particle.scale.z;
    worldMatrix[3].xyz = particle.translate;
    
    //ワールド行列の逆行列を計算
    float32_t4x4 worldInverseTranspose = Transpose(Inverse(worldMatrix));
    
    //出力の計算
    output.position = mul(input.position, mul(worldMatrix, mul(gPerView.viewMatrix, gPerView.projectionMatrix)));
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(input.normal, (float32_t3x3) worldInverseTranspose));
    output.color = particle.color;
    output.worldPosition = worldMatrix[3].xyz;
    output.toEye = normalize(gPerView.worldPosition - output.worldPosition);
    output.cameraToPosition = normalize(output.worldPosition - gPerView.worldPosition);

    return output;
}