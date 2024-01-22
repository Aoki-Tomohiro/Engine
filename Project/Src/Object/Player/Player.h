#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include <optional>

class Player : public IGameObject, Collider
{
public:
	//プレイヤーの状態
	enum class Behavior
	{
		kRoot,//通常状態
		kDash,//ダッシュ状態
		kAttack,//攻撃状態
		kAirAttack,//空中攻撃状態
		kJump,//ジャンプ状態
		kKnockBack,//ノックバック状態
	};

	//ダッシュ用ワーク
	struct WorkDash
	{
		//ダッシュ用の媒介変数
		int dashParameter = 0;
		uint32_t coolTime = 0;
		const uint32_t dashCoolTime = 30;
	};

	//攻撃用ワーク
	struct WorkAttack
	{
		//攻撃ギミックの媒介変数
		Vector3 translation{};
		Vector3 rotation{};
		uint32_t attackParameter = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
	};

	//コンボの数
	static const int ComboNum = 4;
	static const int airComboNum = 4;

	//攻撃用定数
	struct ConstAttack
	{
		//アニメーション開始座標
		Vector3 startPosition{};
		//アニメーション開始角度
		Vector3 startRotation{};
		//振りかぶりの時間
		uint32_t anticipationTime;
		//ための時間
		uint32_t chargeTime;
		//攻撃振りの時間
		uint32_t swingTime;
		//硬直時間
		uint32_t recoveryTime;
		//振りかぶりの移動速さ
		Vector3 anticipationSpeed;
		Vector3 anticipationRotateSpeed;
		//ための移動速さ
		Vector3 chargeSpeed;
		Vector3 chargeRotateSpeed;
		//攻撃降りの移動速さ
		Vector3 swingSpeed;
		Vector3 swingRotateSpeed;
		//移動速度
		Vector3 velocity;
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camemra) override;

	void DrawUI() override;

	void OnCollision(Collider* collider) override;

	const Vector3 GetWorldPosition() const override;

	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void Move(const float moveSpeed);

private:
	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	Behavior behavior_ = Behavior::kRoot;

	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	Vector3 velocity_{};

	Vector3 gravity_{};

	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	const Camera* camera_ = nullptr;
};

