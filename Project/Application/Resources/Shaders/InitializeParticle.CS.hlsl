#include "Particle.hlsli"

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);

[numthreads(1024,1,1)]
void main(uint32_t3 DTid : SV_DispatchThreadID)
{
    uint32_t particleIndex = DTid.x;
    //パーティクルの初期化
    if(particleIndex < kMaxParticles)
    {
        //Particle構造体の全要素を0で埋めるという書き方
        gParticles[particleIndex] = (Particle) 0;
        gParticles[particleIndex].translate = float32_t3(1.0f, 1.0f, 1.0f);
        gParticles[particleIndex].scale = float32_t3(0.5f, 0.5f, 0.5f);
        gParticles[particleIndex].lifeTime = 1.0f;
        gParticles[particleIndex].velocity = float32_t3(0.0f, 0.0f, 0.0f);
        gParticles[particleIndex].currentTime = 0.0f;
        gParticles[particleIndex].color = float32_t4(1.0f, 1.0f, 1.0f, 0.0f);
        //FreeListを連番で初期化
        gFreeList[particleIndex] = particleIndex;

    }
    
    //Counterの初期化
    if(particleIndex == 0)
    {
        gFreeListIndex[0] = kMaxParticles - 1;
    }
}