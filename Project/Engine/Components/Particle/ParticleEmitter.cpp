#include "ParticleEmitter.h"
#include "Engine/Utilities/GameTimer.h"

void ParticleEmitter::Initialize(const std::string& name)
{
	//名前の初期化
	name_ = name;

	//EmitterResourceの作成
	emitterResource_ = std::make_unique<UploadBuffer>();
	emitterResource_->Create(sizeof(EmitterSphere));
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
	
	//EmitterResourceに書き込む
	EmitterSphere* emitterSphereData = static_cast<EmitterSphere*>(emitterResource_->Map());
	emitterSphereData->translate = translate_;
	emitterSphereData->radius = radius_;
	emitterSphereData->count = count_;
	emitterSphereData->emit = emit_;
	emitterResource_->Unmap();
}