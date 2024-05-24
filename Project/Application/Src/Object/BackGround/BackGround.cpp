#include "BackGround.h"

void BackGround::Initialize(Skybox* skybox)
{
	//Skyboxの初期化
	skybox_ = skybox;

	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	worldTransform_.scale_ = { 500.0f,500.0f,500.0f };
}

void BackGround::Update()
{
	worldTransform_.UpdateMatrixFromEuler();
}

void BackGround::Draw(const Camera& camera)
{
	skybox_->Draw(worldTransform_, camera);
}