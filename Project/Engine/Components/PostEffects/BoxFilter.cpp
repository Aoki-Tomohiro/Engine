#include "BoxFilter.h"

void BoxFilter::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataBoxFilter));
	Update();
}

void BoxFilter::Update()
{
	ConstBuffDataBoxFilter* grayScaleData = static_cast<ConstBuffDataBoxFilter*>(constBuff_->Map());
	grayScaleData->isEnable = isEnable_;
	grayScaleData->kernelSize = kernelSize_;
	constBuff_->Unmap();
}