#pragma once
#include "Engine/Framework/IGameObject.h"
#include "Engine/Components/Collider.h"

class Boss : public IGameObject, public Collider 
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
	void Draw(const Camera& camera);

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

private:
	//当たり判定のフラグ
	bool onCollision_ = false;
	bool preOnCollision_ = false;

	//ヒットフラグ
	bool isHit_ = false;

	//シェイクタイマー
	static const uint32_t kShakeTime = 10;
	uint32_t shakeTimer_ = 0;

	//アニメーション用のワールドトランスフォーム
	WorldTransform worldTransformAnimation_{};

	//ノックバック時の速度
	Vector3 knockBackVelocity_{};
	//ノックバックのフラグ
	bool isKnockBack_ = false;
	uint32_t knockBackTimer_ = 0;
	uint32_t knockBackTime_ = 20;
};

