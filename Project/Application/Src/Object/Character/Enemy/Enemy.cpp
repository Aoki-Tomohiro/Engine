#include "Enemy.h"

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