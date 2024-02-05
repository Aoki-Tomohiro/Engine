#include "Ground.h"

void Ground::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.translation_.y = -1.0f;
	worldTransform_.scale_ = { 20.0f,1.0f,20.0f };
}

void Ground::Update()
{
	worldTransform_.UpdateMatrixFromEuler();
}

void Ground::Draw(const Camera& camera)
{
	if (model_)
	{
		model_->Draw(worldTransform_, camera);
	}
}

void Ground::DrawUI()
{

}