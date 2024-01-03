#pragma once
#include "Engine/Framework/IGameObject.h"
#include "Engine/Components/Collider.h"
#include "Engine/Components/Input.h"
#include "Weapon.h"
#include <array>
#include <optional>

class Player : public IGameObject, public Collider
{
public:
	//プレイヤーの状態
	enum class Behavior
	{
		kRoot,//通常状態
		kDash,//ダッシュ状態
		kAttack,//攻撃状態
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
	static const int ComboNum = 10;

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
		//入力受付時間
		uint32_t inputTime;
		//振りかぶりの移動速さ
		Vector3 anticipationSpeed;
		Vector3 anticipationRotateSpeed;
		//ための移動速さ
		Vector3 chargeSpeed;
		Vector3 chargeRotateSpeed;
		//攻撃降りの移動速さ
		Vector3 swingSpeed;
		Vector3 swingRotateSpeed;
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camemra"></param>
	void Draw(const Camera& camemra) override;

	/// <summary>
	/// パーティクルの更新
	/// </summary>
	void UpdateParticle();

	/// <summary>
	/// パーティクルの描画
	/// </summary>
	/// <param name="camera"></param>
	void DrawParticle(const Camera& camera);

	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnCollision(Collider* collider) override;

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	Vector3 GetWorldPosition() override;

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns></returns>
	WorldTransform& GetWorldTransform() override { return worldTransform_; };

	/// <summary>
	/// カメラ設定
	/// </summary>
	/// <param name="camera"></param>
	void SetCamera(const Camera* camera) { camera_ = camera; };

	/// <summary>
	/// 武器を取得
	/// </summary>
	/// <returns></returns>
	Weapon* GetWeapon() { return weapon_; };

	/// <summary>
	/// 現在のコンボのインデックスを取得
	/// </summary>
	/// <returns></returns>
	int32_t GetComboIndex() { return workAttack_.comboIndex; };

	/// <summary>
	/// ImGuiの更新
	/// </summary>
	void UpdateImGui();

private:
	/// <summary>
	/// 通常行動初期化
	/// </summary>
	void BehaviorRootInitialize();

	/// <summary>
	/// 通常行動更新
	/// </summary>
	void BehaviorRootUpdate();

	/// <summary>
	/// ダッシュ行動初期化
	/// </summary>
	void BehaviorDashInitialize();

	/// <summary>
	/// ダッシュ行動更新
	/// </summary>
	void BehaviorDashUpdate();

	/// <summary>
	/// ジャンプ行動初期化
	/// </summary>
	void BehaviorJumpInitialize();

	/// <summary>
	/// ジャンプ行動更新
	/// </summary>
	void BehaviorJumpUpdate();

	/// <summary>
	/// 攻撃行動初期化
	/// </summary>
	void BehaviorAttackInitialize();

	/// <summary>
	/// 攻撃行動更新
	/// </summary>
	void BehaviorAttackUpdate();

	/// <summary>
	/// ノックバック行動の初期化
	/// </summary>
	void BehaviorKnockBackInitialize();

	/// <summary>
	/// ノックバック行動の更新
	/// </summary>
	void BehaviorKnockBackUpdate();

	/// <summary>
	/// 急接近行動の初期化
	/// </summary>
	void BehaviorRapidApproachInitialize();

	/// <summary>
	/// 急接近行動の更新
	/// </summary>
	void BehaviorRapidApproachUpdate();

	/// <summary>
	/// 回転
	/// </summary>
	void Rotate(const Vector3& v);

	/// <summary>
	/// 攻撃モーション
	/// </summary>
	void AttackAnimation();

private:
	//入力クラス
	Input* input_ = nullptr;

	//振る舞い
	Behavior behavior_ = Behavior::kRoot;

	//次のふるまいのリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	//カメラ
	const Camera* camera_ = nullptr;

	//速度
	Vector3 velocity_{};

	//クォータニオン
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//ダッシュ用の変数
	WorkDash workDash_{};

	//コンボ定数表
	static std::array<ConstAttack, ComboNum> kConstAttacks_;

	//攻撃用の変数
	WorkAttack workAttack_{};

	//ノックバック時の速度
	Vector3 knockBackVelocity_{};

	//武器
	Model* modelWeapon_ = nullptr;
	Weapon* weapon_ = nullptr;

	int attackIndex_ = 0;

	Vector3 targetPosition_{ 0.0f,0.0f,0.0f };
	Vector3 rapidApproachVelocity_{ 0.0f,0.0f,0.0f };
};

