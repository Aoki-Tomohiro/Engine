#include "Particle.hlsli"

struct EmitterSphere
{
    float32_t3 translate; //位置
    float32_t radius; //射出半径
    uint32_t count; //射出数
    uint32_t emit; //射出許可
};

float32_t rand3dTo1d(float32_t3 value, float32_t3 dotDir = float32_t3(12.9898, 78.233, 37.719))
{
    float32_t3 smallValue = sin(value);
    float32_t random = dot(smallValue, dotDir);
    random = frac(sin(random) * 143758.5453);
    return random;
}

float32_t3 rand3dTo3d(float32_t3 value)
{
    return float32_t3(
        rand3dTo1d(value, float32_t3(12.989, 78.233, 37.719)),
        rand3dTo1d(value, float32_t3(39.346, 11.135, 83.155)),
        rand3dTo1d(value, float32_t3(73.156, 52.235, 09.151))
    );
}

class RandomGenerator
{
    float32_t3 seed;
    float32_t3 Generate3d()
    {
        seed = rand3dTo3d(seed);
        return seed;
    }
    float32_t Generate1d()
    {
        float32_t result = rand3dTo1d(seed);
        seed.x = result;
        return result;
    }
};

RWStructuredBuffer<Particle> gParticles : register(u0);
RWStructuredBuffer<int32_t> gFreeCounter : register(u1);
ConstantBuffer<EmitterSphere> gEmitter : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

//今回スレッド数は1。複数のEmitterを扱い、同時に処理したいような場合は敵宜スレッド数を増やすと良い
[numthreads(1, 1, 1)]
void main(uint32_t DTid : SV_DispatchThreadID)
{
    //コンストラクタはないので、このように初期化すると良い。構造体的に初期化することは可能
    RandomGenerator generator;
    generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
    //射出許可が出ていたら
    if(gEmitter.emit)
    {
        //Generate3d呼ぶたびにseedが変わるので結果すべての乱数が変わる
        for (uint32_t countIndex = 0; countIndex < gEmitter.count; ++countIndex)
        {
            int32_t particleIndex;
            //gFreeCounter[0]に1を足し、足す前の値をparticleIndexに格納する
            InterlockedAdd(gFreeCounter[0], 1, particleIndex);
            //最大数よりもparticleの数が少なければ射出可能
            if(particleIndex < kMaxParticles)
            {
                //Particleの初期化
                gParticles[particleIndex].translate = generator.Generate3d();
                gParticles[particleIndex].scale = generator.Generate3d();
                gParticles[particleIndex].color.rgb = generator.Generate3d();
                gParticles[particleIndex].color.a = 1.0f;
            }
        }
    }
}