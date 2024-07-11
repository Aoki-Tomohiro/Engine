#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/2D/Sprite.h"
#include "States/EnemyStateIdle.h"

class Enemy : public GameObject
{
public:
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
	/// <param name="camera"></param>
	void Draw(const Camera& camera) override;

	/// <summary>
	/// UIの描画
	/// </summary>
	void DrawUI() override;

	/// <summary>
	/// 当たっているの時の処理
	/// </summary>
	/// <param name="gameObject"></param>
	void OnCollision(GameObject* gameObject) override;

	/// <summary>
	/// 当たった瞬間の処理
	/// </summary>
	/// <param name="gameObject"></param>
	void OnCollisionEnter(GameObject* gameObject) override;

	/// <summary>
	/// 離れた瞬間の処理
	/// </summary>
	/// <param name="gameObject"></param>
	void OnCollisionExit(GameObject* gameObject) override;

	/// <summary>
	/// 警告状態科を取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsWarning() const { return state_->GetIsWarning(); };

	/// <summary>
	/// 攻撃状態かを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsAttack() const { return state_->GetIsAttack(); };

	/// <summary>
	/// HPを取得
	/// </summary>
	/// <returns></returns>
	const float GetHP() const { return hp_; };

	/// <summary>
	/// ColliderのOffset値を取得
	/// </summary>
	/// <returns></returns>
	const Vector3& GetColliderOffset() const { return colliderOffset_; };

	/// <summary>
	/// TimeScaleを設定
	/// </summary>
	/// <param name="timeScale"></param>
	void SetTimeScale(const float timeScale) { timeScale_ = timeScale; };

	/// <summary>
	/// タイトルシーンのフラグを設定
	/// </summary>
	/// <param name="isInTitleScene"></param>
	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

private:
	/// <summary>
	/// 状態の遷移
	/// </summary>
	/// <param name="state"></param>
	void ChangeState(IEnemyState* state);

private:
	//プレイヤーの状態
	std::unique_ptr<IEnemyState> state_ = nullptr;

	//速度
	Vector3 velocity{};

	//TimeScale
	float timeScale_ = 1.0f;

	//Quaternion
	Quaternion destinationQuaternion_{ 0.0f,0.0f,0.0f,1.0f };

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//Debugモードなのか
	bool isDebug_ = false;

	//アニメーションの時間
	float animationTime_ = 0.0f;

	//Collider
	Vector3 colliderOffset_{ 0.0f,4.0f,0.0f };

	//HP
	std::unique_ptr<Sprite> spriteHpBar_ = nullptr;
	std::unique_ptr<Sprite> spriteHpBarFrame_ = nullptr;
	Vector2 hpBarSize_{ 480.0f,16.0f };
	const float kMaxHP = 800.0f;
	float hp_ = kMaxHP;

	float disolveParameter_ = 0.0f;

	//フレンドクラスに登録
	friend class EnemyStateIdle;
	friend class EnemyStateTackle;
};

