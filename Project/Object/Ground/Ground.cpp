#include "Ground.h"

void Ground::Initialize() 
{
	//基底クラスの初期化
	IGameObject::Initialize();
	worldTransform_.translation_.y = -2.0f;
	worldTransform_.scale_ = { 50.0f,2.0f,50.0f };
}

void Ground::Update()
{
	//基底クラスの更新
	IGameObject::Update();
}

void Ground::Draw(const Camera& camera) 
{
	//基底クラスの描画
	IGameObject::Draw(camera);
}