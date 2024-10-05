#include "Particle.hlsli"

struct EmitterSphere
{
    float32_t3 translate;     //位置
    float32_t radius;         //射出半径
    uint32_t count;           //射出数
    uint32_t emit;            //射出許可
    float32_t3 rotateMin;     //スケールの最小値
    float32_t3 rotateMax;     //スケールの最大値
    float32_t4 quaternion;    //Quaternion
    float32_t3 scaleMin;      //スケールの最小値
    float32_t3 scaleMax;      //スケールの最大値
    float32_t3 velocityMin;   //速度の最小値
    float32_t3 velocityMax;   //速度の最大値
    float32_t lifeTimeMin;    //寿命の最小値
    float32_t lifeTimeMax;    //寿命の最大値
    float32_t4 colorMin;      //色の最小値
    float32_t4 colorMax;      //色の最大値
    int32_t alignToDirection; //進行方向に回転させるか
};

struct EmitterInformation
{
    int32_t numEmitters;
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
RWStructuredBuffer<int32_t> gFreeListIndex : register(u1);
RWStructuredBuffer<uint32_t> gFreeList : register(u2);
StructuredBuffer<EmitterSphere> gEmitter : register(t0);
ConstantBuffer<EmitterInformation> gEmitterInformation : register(b0);
ConstantBuffer<PerFrame> gPerFrame : register(b1);

[numthreads(1024, 1, 1)]
void main(uint32_t DTid : SV_DispatchThreadID)
{
    //Emitterのインデックスを取得
    uint32_t emitterIndex = DTid.x;
    
    //Emitterの上限を超えていなかったらパーティクルを生成する
    if(emitterIndex < gEmitterInformation.numEmitters)
    {
        //コンストラクタはないので、このように初期化すると良い。構造体的に初期化することは可能
        RandomGenerator generator;
        generator.seed = (DTid + gPerFrame.time) * gPerFrame.time;
        
        //射出許可が出ていたら
        if (gEmitter[emitterIndex].emit)
        {
            //Generate3d呼ぶたびにseedが変わるので結果すべての乱数が変わる
            for (uint32_t countIndex = 0; countIndex < gEmitter[emitterIndex].count; ++countIndex)
            {
                int32_t freeListIndex;
                //FreeListのIndexを一つ前に設定し、現在のIndexを取得する
                InterlockedAdd(gFreeListIndex[0], -1, freeListIndex);
                if(0 <= freeListIndex && freeListIndex < kMaxParticles)
                {
                    uint32_t particleIndex = gFreeList[freeListIndex];
                    
                    //位置の初期化
                    float32_t theta = rand3dTo1d(generator.Generate3d()) * 2.0 * 3.141592653589793;
                    float32_t phi = acos(2.0 * rand3dTo1d(generator.Generate3d() + float32_t3(1.0, 1.0, 1.0)) - 1.0);
                    float32_t3 direction = { sin(phi) * cos(theta), sin(phi) * sin(theta), cos(phi) };
                    float32_t distance = pow(generator.Generate1d(), 1.0 / 3.0) * gEmitter[emitterIndex].radius;
                    gParticles[particleIndex].translate = gEmitter[emitterIndex].translate + direction * distance;
                
                    //スケールの初期化
                    gParticles[particleIndex].scale = lerp(gEmitter[emitterIndex].scaleMin, gEmitter[emitterIndex].scaleMax, generator.Generate3d());
                    
                    //回転の初期化
                    gParticles[particleIndex].rotate = lerp(gEmitter[emitterIndex].rotateMin, gEmitter[emitterIndex].rotateMax, generator.Generate3d());
                    
                    gParticles[particleIndex].quaternion = gEmitter[emitterIndex].quaternion;
                
                    //寿命の初期化
                    gParticles[particleIndex].lifeTime = lerp(gEmitter[emitterIndex].lifeTimeMin, gEmitter[emitterIndex].lifeTimeMax, generator.Generate1d());
                
                    //速度の初期化
                    gParticles[particleIndex].velocity = lerp(gEmitter[emitterIndex].velocityMin, gEmitter[emitterIndex].velocityMax, generator.Generate3d());
                
                    //経過時間の初期化
                    gParticles[particleIndex].currentTime = 0.0f;
                
                    //色の初期化
                    gParticles[particleIndex].color = lerp(gEmitter[emitterIndex].colorMin, gEmitter[emitterIndex].colorMax, float32_t4(generator.Generate3d(), generator.Generate1d()));
               
                    //進行方向に回転させるかどうかを設定
                    gParticles[particleIndex].alignToDirection = gEmitter[emitterIndex].alignToDirection;
                }
                else
                {
                    //発生させられなかったので、減らしてしまった分元に戻す。これを忘れると発生させられなかった分だけどんどんIndexが減ってしまう
                    InterlockedAdd(gFreeListIndex[0], 1);
                    //Emit中にParticleは消えないので、この後発生することはないためbreakして終わらせる
                    break;
                }
            }
        }
    }
}