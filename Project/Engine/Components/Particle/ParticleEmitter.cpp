#include "ParticleEmitter.h"
#include "Engine/Utilities/GameTimer.h"

void ParticleEmitter::Initialize(const std::string& name, const float lifeTime)
{
	//名前の初期化
	name_ = name;

	//寿命の初期化
	emitterLifeTime_ = lifeTime;
}

void ParticleEmitter::Update()
{
	//タイムを減算
	frequencyTime_ -= GameTimer::GetDeltaTime();

	//射出間隔を上回ったら射出許可を出して時間を調整
	if (frequencyTime_ <= 0.0f)
	{
		frequencyTime_ = frequency_;
		emit_ = 1;
	}
	//射出間隔を上回っていないので射出許可は出せない
	else
	{
		emit_ = 0;
	}

	//エミッターの寿命を減らす
	lifeTimer_ += GameTimer::GetDeltaTime();

	//寿命が尽きたら死亡フラグを立てる
	if (lifeTimer_ > emitterLifeTime_)
	{
		isDead_ = true;
	}
}