#include "DirectionalLight.h"

void DirectionalLight::Initialize()
{
	constBuff_ = std::make_unique<UploadBuffer>();
	constBuff_->Create(sizeof(ConstBuffDataDirectionalLight));
	Update();
}

void DirectionalLight::Update()
{
	ConstBuffDataDirectionalLight* directionalLightData = static_cast<ConstBuffDataDirectionalLight*>(constBuff_->Map());
	directionalLightData->enableLighting = enableLighting_;
	directionalLightData->lightingMethod = lightingMethod_;
	directionalLightData->color = color_;
	directionalLightData->direction = direction_;
	directionalLightData->intensity = intensity_;
	constBuff_->Unmap();
}