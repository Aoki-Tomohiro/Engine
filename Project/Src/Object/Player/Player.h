#pragma once
#include "Engine/Framework/Object/IGameObject.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Collision/Collider.h"
#include "Engine/Components/Collision/CollisionConfig.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/2D/Sprite.h"
#include "Engine/Math/MathFunction.h"
#include <optional>

class LockOn;

class Player : public IGameObject, public Collider
{
public:
	//プレイヤーの状態
	enum class Behavior
	{
		kRoot,//通常状態
		kDash,//ダッシュ状態
		kAttack,//攻撃状態
		kAirAttack,//空中攻撃
		kJump,//ジャンプ中
		kKnockBack,//ノックバック
		kRapidApproach,//急接近
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

	void SetLockOn(const LockOn* lockOn) { lockOn_ = lockOn; }

private:
	void BehaviorRootInitialize();

	void BehaviorRootUpdate();

	void Move(const float speed);

	void Rotate(const Vector3& v);

private:
	//入力クラス
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;

	//次のふるまいのリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//速度
	Vector3 velocity_{};

	//重力
	Vector3 gravity_{};

	//クォータニオン
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//カメラ
	const Camera* camera_ = nullptr;

	//ロックオン
	const LockOn* lockOn_ = nullptr;

	//体力バー
	std::unique_ptr<Sprite> spriteHpBar_ = nullptr;
	std::unique_ptr<Sprite> spriteHpBarFrame_ = nullptr;

	//HP
	Vector2 hpBarSize_{ 480.0f,16.0f };
	const float kMaxHP = 30.0f;
	float hp_ = kMaxHP;
};

