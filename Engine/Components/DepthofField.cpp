#include "DepthofField.h"

void DepthofField::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataDoF));
	Update();
}

void DepthofField::Update()
{
	ConstBuffDataDoF* dofData = static_cast<ConstBuffDataDoF*>(constBuff_->Map());
	dofData->enable = isEnable_;
	constBuff_->Unmap();
}