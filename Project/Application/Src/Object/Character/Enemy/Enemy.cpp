#include "Enemy.h"

void Enemy::Initialize()
{
	//基底クラスの初期化
	ICharacter::Initialize();
}

void Enemy::Update()
{
	//基底クラスの更新
	ICharacter::Update();
}

void Enemy::DrawUI()
{
	//基底クラスの描画
	ICharacter::DrawUI();
}

void Enemy::InitializeAnimator()
{
	//基底クラスの呼び出し
	ICharacter::InitializeAnimator();
}

void Enemy::InitializeUISprites()
{
	//テクスチャの名前を設定
	hpTextureNames_ = { {
		{"barBack_horizontalLeft.png","barBack_horizontalMid.png","barBack_horizontalRight.png"},
		{"barBlue_horizontalLeft.png","barBlue_horizontalBlue.png","barBlue_horizontalRight.png"},
		}
	};

	//スプライトの座標を設定
	hpBarSegmentPositions_ = { {
		{ {	{711.0f, 40.0f}, {720.0f, 40.0f}, {1200.0f, 40.0f},}},
		{ { {711.0f, 40.0f}, {720.0f, 40.0f}, {1200.0f, 40.0f},}},
		}
	};

	//基底クラスの呼び出し
	ICharacter::InitializeUISprites();
}