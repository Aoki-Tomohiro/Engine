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
	constBuff_->Unmap();
}