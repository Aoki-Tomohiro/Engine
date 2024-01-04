#include "Bloom.h"

void Bloom::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataBloom));
	Update();
}

void Bloom::Update()
{
	ConstBuffDataBloom* bloomData = static_cast<ConstBuffDataBloom*>(constBuff_->Map());
	bloomData->enable = isEnable_;
	bloomData->intensity = intensity_;
	bloomData->highIntensityColorWeight = highIntensityColorWeight_;
	bloomData->highIntensityBlurColorWeight = highIntensityBlurColorWeight_;
	bloomData->highIntensityShrinkBlurColorWeight = highIntensityShrinkBlurColorWeight_;
	constBuff_->Unmap();
}