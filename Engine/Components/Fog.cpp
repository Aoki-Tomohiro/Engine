#include "Fog.h"

void Fog::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataFog));
	Update();
}

void Fog::Update()
{
	ConstBuffDataFog* fogData = static_cast<ConstBuffDataFog*>(constBuff_->Map());
	fogData->enable = isEnable_;
	fogData->scale = scale_;
	fogData->attenuationRate = attenuationRate_;
	constBuff_->Unmap();
}