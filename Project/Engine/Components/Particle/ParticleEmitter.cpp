#include "ParticleEmitter.h"
#include "Engine/Utilities/GameTimer.h"

void ParticleEmitter::Initialize(const std::string& name, const float lifeTime)
{
	//名前の初期化
	name_ = name;

	//エミッターの寿命の初期化
	deathTimer_ = lifeTime;
}

void ParticleEmitter::Update()
{
	//タイムを加算
	frequencyTime_ += GameTimer::GetDeltaTime();

	//射出間隔を上回ったら射出許可を出して時間を調整
	if (frequency_ <= frequencyTime_)
	{
		frequencyTime_ -= frequency_;
		emit_ = 1;
	}
	//射出間隔を上回っていないので射出許可は出せない
	else
	{
		emit_ = 0;
	}

	//エミッターの寿命を減らす
	deathTimer_ -= GameTimer::GetDeltaTime();

	//寿命が尽きたら死亡フラグを立てる
	if (deathTimer_ < 0)
	{
		isDead_ = true;
	}
}