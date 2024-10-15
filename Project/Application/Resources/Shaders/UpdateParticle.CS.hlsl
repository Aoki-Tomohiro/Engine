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
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);
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
        //加速度フィールドの処理
        for (uint32_t i = 0; i < gAccelerationFieldCount.numAccelerationFields; ++i)
        {
            AccelerationField field = gAccelerationFields[i];
            if (gParticles[particleIndex].translate.x >= field.translate.x + field.min.x &&
                gParticles[particleIndex].translate.y >= field.translate.y + field.min.y &&
                gParticles[particleIndex].translate.z >= field.translate.z + field.min.z &&
                gParticles[particleIndex].translate.x <= field.translate.x + field.max.x &&
                gParticles[particleIndex].translate.y <= field.translate.y + field.max.y &&
                gParticles[particleIndex].translate.z <= field.translate.z + field.max.z)
            {
                gParticles[particleIndex].velocity += field.acceleration * gPerFrame.deltaTime;
            }
        }
            
        //重力フィールドの処理
        for (uint32_t j = 0; j < gGravityFieldCount.numGravityFields; ++j)
        {
            GravityField field = gGravityFields[j];
            if (gParticles[particleIndex].translate.x >= field.translate.x + field.min.x &&
                gParticles[particleIndex].translate.y >= field.translate.y + field.min.y &&
                gParticles[particleIndex].translate.z >= field.translate.z + field.min.z &&
                gParticles[particleIndex].translate.x <= field.translate.x + field.max.x &&
                gParticles[particleIndex].translate.y <= field.translate.y + field.max.y &&
                gParticles[particleIndex].translate.z <= field.translate.z + field.max.z)
            {
                float32_t3 direction = field.translate - gParticles[particleIndex].translate;
                float32_t distance = length(direction);
                if (distance > field.stopDistance)
                {
                    gParticles[particleIndex].velocity += normalize(direction) * field.strength * gPerFrame.deltaTime;
                }
                else
                {
                    gParticles[particleIndex].velocity *= 0.9f;
                }
            }
        }
         
        //パーティクルの更新処理
        float32_t3 currentTranslate = gParticles[particleIndex].translate;
        gParticles[particleIndex].translate += gParticles[particleIndex].velocity;
        
        //パーティクルが進行方向に向くように設定されているかを確認
        if (gParticles[particleIndex].alignToDirection)
        {
            //クォータニオンの更新
            gParticles[particleIndex].quaternion = LookAt(currentTranslate, gParticles[particleIndex].translate);
        }
        else
        {
            //クォータニオンの初期化
            gParticles[particleIndex].quaternion = float32_t4(0.0f, 0.0f, 0.0f, 1.0f);
            
            //回転させる
            gParticles[particleIndex].rotate += gParticles[particleIndex].rotSpeed;
        }
        
        //現在の時間を進める
        gParticles[particleIndex].currentTime += gPerFrame.deltaTime;
        //イージング係数を計算
        float32_t easingParameter = saturate(gParticles[particleIndex].currentTime / gParticles[particleIndex].lifeTime);
        //色の更新
        gParticles[particleIndex].color.rgb = lerp(gParticles[particleIndex].initialColor, gParticles[particleIndex].targetColor, easingParameter);
        //アルファの更新
        gParticles[particleIndex].color.a = saturate(lerp(gParticles[particleIndex].initialAlpha, gParticles[particleIndex].targetAlpha, easingParameter));
        //スケールの更新
        gParticles[particleIndex].scale = lerp(gParticles[particleIndex].initialScale, gParticles[particleIndex].targetScale, easingParameter);
        
        //寿命が尽きたので、ここはFreeとする
        if (gParticles[particleIndex].currentTime >= gParticles[particleIndex].lifeTime)
        {
            //スケールに0を入れておいてVertexShader出力で棄却されるようにする
            gParticles[particleIndex].scale = float32_t3(0.0f, 0.0f, 0.0f);
            int32_t freeListIndex;
            InterlockedAdd(gFreeListIndex[0], 1, freeListIndex);
            
            //最新のFreeListIndexの場所に死んだParticleのIndexを設定する。
            if((freeListIndex + 1) < kMaxParticles)
            {
                gFreeList[freeListIndex + 1] = particleIndex;
            }
            else
            {
                //ここに来るはずはない。来たら何かが間違っているが、安全策をうっておく
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
            }
        }
    }
}