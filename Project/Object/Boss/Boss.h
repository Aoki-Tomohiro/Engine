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
	Vector3 GetWorldPosition() override;

	/// <summary>
	/// ワールド変換データを取得
	/// </summary>
	/// <returns></returns>
	WorldTransform& GetWorldTransform() override { return worldTransform_; };

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
};

