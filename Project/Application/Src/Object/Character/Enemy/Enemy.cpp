#include "Enemy.h"

void Enemy::Initialize()
{
	//基底クラスの初期化
	BaseCharacter::Initialize();

	//HPの初期化
	maxHp_ = 800.0f;
	hp_ = maxHp_;

	//状態の初期化
	ChangeState("Idle");
}

void Enemy::Update()
{
	//タイトルシーンにいる場合は基底クラスの更新だけする
	if (isInTitleScene_)
	{
		GameObject::Update();
		return;
	}

	//基底クラスの更新
	BaseCharacter::Update();
}

void Enemy::OnCollision(GameObject* gameObject)
{
	//状態の衝突判定処理
	(void*)gameObject;
	//currentState_->OnCollision(gameObject);
}

void Enemy::InitializeAnimator()
{
	//基底クラスの呼び出し
	BaseCharacter::InitializeAnimator();

	//アニメーション名とファイルパス
	std::vector<std::pair<std::string, std::string>> animations = {
		{"Idle", "Enemy/Animations/Idle.gltf"}, {"Walk1", "Enemy/Animations/Walk1.gltf"}, {"Walk2", "Enemy/Animations/Walk2.gltf"}, {"Walk3", "Enemy/Animations/Walk3.gltf"},
		{"Walk4", "Enemy/Animations/Walk4.gltf"}, {"Run", "Enemy/Animations/Run.gltf"}, {"Dash", "Enemy/Animations/Dash.gltf"}, {"TackleAttack", "Enemy/Animations/Tackle.gltf"},
		{"JumpAttack", "Enemy/Animations/JumpAttack.gltf"}, {"ComboAttack", "Enemy/Animations/ComboAttack.gltf"}, {"EarthSpike", "Enemy/Animations/EarthSpike.gltf"},
		{"LaserBeam", "Enemy/Animations/LaserBeam.gltf"}, {"HitStun", "Enemy/Animations/HitStun.gltf"}, {"Knockdown", "Enemy/Animations/Knockdown.gltf"},
		{"StandUp", "Enemy/Animations/StandUp.gltf"}, {"Death", "Enemy/Animations/Death.gltf"}
	};

	//アニメーションを追加
	for (const auto& [name, path] : animations)
	{
		animator_->AddAnimation(name, AnimationManager::Create(path));
	}

	//通常アニメーションを再生
	animator_->PlayAnimation("Idle", 1.0f, true);
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
	BaseCharacter::InitializeUISprites();
}