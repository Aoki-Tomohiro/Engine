#pragma once
#include "Engine/Framework/IGameObject.h"
#include "Engine/2D/Sprite.h"
#include "Project/Object/Boss/States/BossStateNormal.h"
#include "Project/Object/Boss/States/BossStateTackle.h"
#include "Project/Object/Boss/States/BossStateMissileAttack.h"
#include "Project/Object/Boss/States/BossStateLaserAttack.h"
#include "Project/Object/Boss/States/BossStateCrashDown.h"
#include "Project/Object/Boss/Missile.h"
#include "Project/Object/Boss/Laser.h"
#include <list>
#include <memory>

class Boss : public IGameObject, public Collider 
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Boss();

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
	void Draw(const Camera& camera);

	/// <summary>
	/// UIの描画
	/// </summary>
	void DrawUI();

	/// <summary>
	/// 状態遷移
	/// </summary>
	/// <param name="newState"></param>
	void ChangeState(IBossState* newState);

	/// <summary>
	/// ミサイルを取得
	/// </summary>
	/// <returns></returns>
	const std::list<std::unique_ptr<Missile>>& GetMissiles() const { return missiles_; };

	/// <summary>
	/// ミサイルを追加
	/// </summary>
	/// <param name="missile"></param>
	void AddMissile(Missile* missile) { missiles_.push_back(std::unique_ptr<Missile>(missile)); };

	/// <summary>
	/// レーザーを取得
	/// </summary>
	/// <returns></returns>
	const std::list<std::unique_ptr<Laser>>& GetLasers() const { return lasers_; };

	/// <summary>
	/// レーザーを追加
	/// </summary>
	/// <param name="laser"></param>
	void AddLaser(Laser* laser) { lasers_.push_back(std::unique_ptr<Laser>(laser)); };

	/// <summary>
	/// 衝突判定
	/// </summary>
	void OnCollision(Collider* collider) override;

	/// <summary>
	/// ワールド座標を取得
	/// </summary>
	/// <returns></returns>
	const Vector3 GetWorldPosition() const override;

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns></returns>
	const WorldTransform& GetWorldTransform() const override { return worldTransform_; };

	/// <summary>
	/// 攻撃フラグを取得
	/// </summary>
	/// <returns></returns>
	const bool GetIsAttack() const { return isAttack_; };

	/// <summary>
	/// 攻撃フラグを設定
	/// </summary>
	/// <param name="isAttack"></param>
	void SetIsAttack(const bool isAttack) { isAttack_ = isAttack; };

	/// <summary>
	/// 前のフレームの座標を取得
	/// </summary>
	/// <returns></returns>
	const Vector3& GetOldPosition() const { return oldPosition_; };

	/// <summary>
	/// HPを取得
	/// </summary>
	/// <returns></returns>
	const float GetHP() const { return hp_; };

	/// <summary>
	/// ダメージを取得
	/// </summary>
	/// <returns></returns>
	const float GetDamage() const { return damage_; };

	/// <summary>
	/// ダメージを設定
	/// </summary>
	/// <param name="damage"></param>
	void SetDamage(const float damage) { damage_ = damage; };

private:
	//状態
	IBossState* state_ = nullptr;

	//ミサイル
	std::list<std::unique_ptr<Missile>> missiles_{};

	//レーザー
	std::list<std::unique_ptr<Laser>> lasers_{};

	//当たり判定のフラグ
	bool onCollision_ = false;
	bool preOnCollision_ = false;

	//攻撃フラグ
	bool isAttack_ = false;

	//前のフレームの座標
	Vector3 oldPosition_{};

	//体力バー
	std::unique_ptr<Sprite> spriteHpBar_ = nullptr;
	std::unique_ptr<Sprite> spriteHpBarFrame_ = nullptr;

	//HP
	Vector2 hpBarSize_{ 480.0f,16.0f };
	const float kMaxHP = 600.0f;
	float hp_ = kMaxHP;

	//ダメージ
	float damage_ = 0.0f;
};

