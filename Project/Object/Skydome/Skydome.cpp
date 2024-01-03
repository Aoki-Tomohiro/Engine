#include "Skydome.h"

void Skydome::Initialize() 
{
	//基底クラスの初期化
	IGameObject::Initialize();
}

void Skydome::Update()
{
	//基底クラスの更新
	IGameObject::Update();
}

void Skydome::Draw(const Camera& camera) 
{
	//基底クラスの描画
	IGameObject::Draw(camera);
}