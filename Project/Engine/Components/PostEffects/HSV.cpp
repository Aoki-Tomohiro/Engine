#include "HSV.h"

void HSV::Initialize()
{
	//ConstBufferの生成
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataHSV));
	Update();
}

void HSV::Update()
{
	//Resourceに書き込む
	ConstBuffDataHSV* hsvData = static_cast<ConstBuffDataHSV*>(constBuff_->Map());
	hsvData->hue = hue_;
	hsvData->saturation = saturation_;
	hsvData->value = value_;
	constBuff_->Unmap();
}