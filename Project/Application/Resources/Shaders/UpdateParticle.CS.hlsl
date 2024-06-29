#include "Particle.hlsli"

struct AccelerationField
{
    float32_t3 acceleration; //加速度
    float32_t3 translate;//位置
    float32_t3 min; //最小範囲
    float32_t3 max;//最大範囲
};

struct AccelerationFieldInformation
{
    int32_t numAccelerationFields;
};

struct GravityField
{
    float32_t3 translate; //位置
    float32_t3 min; //最小範囲
    float32_t3 max; //最大範囲
    float32_t strength;//重力の強さ
    float32_t stopDistance;//動きを止める中心点からの距離
};

struct GravityFieldInformation
{
    int32_t numGravityFields;
};

RWStructuredBuffer<Particle> gParticles : register(u0);
StructuredBuffer<AccelerationField> gAccelerationFields : register(t0);
StructuredBuffer<GravityField> gGravityFields : register(t1);
ConstantBuffer<AccelerationFieldInformation> gAccelerationFieldCount : register(b0);
ConstantBuffer<GravityFieldInformation> gGravityFieldCount : register(b1);
ConstantBuffer<PerFrame> gPerFrame : register(b2);

[numthreads(1024, 1, 1)]
void main(uint32_t DTid : SV_DispatchThreadID )
{
    uint32_t particleIndex = DTid.x;
    if(particleIndex < kMaxParticles)
    {
        //Alphaが0のparticleは死んでいるとみなして更新しない
        if(gParticles[particleIndex].color.a != 0.0f)
        {
            //加速度フィールドの処理
            for (uint32_t i = 0; i < gAccelerationFieldCount.numAccelerationFields; ++i)
            {
                AccelerationField field = gAccelerationFields[i];
                if (all(gParticles[particleIndex].translate >= field.translate + field.min) && all(gParticles[particleIndex].translate <= field.translate + field.max))
                {
                    gParticles[particleIndex].velocity += field.acceleration * gPerFrame.deltaTime;
                }
            }
            
            //重力フィールドの処理
            for (uint32_t j = 0; j < gGravityFieldCount.numGravityFields; ++j)
            {
                GravityField field = gGravityFields[j];
                if (all(gParticles[particleIndex].translate >= field.translate + field.min) && all(gParticles[particleIndex].translate <= field.translate + field.max))
                {
                    float32_t3 direction = field.translate - gParticles[particleIndex].translate;
                    float32_t distance = length(direction);
                    if (distance > field.stopDistance)
                    {
                        direction = normalize(direction);
                        gParticles[particleIndex].velocity += direction * field.strength * gPerFrame.deltaTime;
                    }
                    else
                    {
                        gParticles[particleIndex].velocity *= 0.9f;
                    }
                }
            }
            
            gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
            gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
            float32_t alpha = 1.0f - (gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
            gParticles[particleIndex].color.a = saturate(alpha);
        }
    }
}