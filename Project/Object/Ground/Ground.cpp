#include "Ground.h"

void Ground::Initialize() 
{
	//基底クラスの初期化
	IGameObject::Initialize();
	worldTransform_.translation_.y = -2.0f;
	worldTransform_.scale_ = { 50.0f,2.0f,50.0f };

	//地面の表面のワールドトランスフォームの初期化
	groundSurfaceWorldTransform_.Initialize();
	groundSurfaceWorldTransform_.translation_.y = 0.01f;
	groundSurfaceWorldTransform_.scale_ = { 50.0f,1.0f,50.0f };
}

void Ground::Update()
{
	//基底クラスの更新
	IGameObject::Update();

	//地面の表面のワールドトランスフォームの初期化
	groundSurfaceWorldTransform_.UpdateMatrixFromEuler();
}

void Ground::Draw(const Camera& camera) 
{
	//基底クラスの描画
	IGameObject::Draw(camera);
	//地面の表面のモデルを描画
	groundSurfaceModel_->Draw(groundSurfaceWorldTransform_, camera);
}