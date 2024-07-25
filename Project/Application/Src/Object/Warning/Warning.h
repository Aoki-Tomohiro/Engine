#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"

class Warning : public GameObject
{
public:
	enum WarningPrimitive
	{
		kBox,
		kSphere,
	};

	//警告用オブジェクトの構造体
	struct BoxWarningObjectSettings
	{
		WarningPrimitive warningPrimitive; // 警告用オブジェクトの形状
		Vector3 position;                  // 座標
		Quaternion quaternion;             // 回転
		Vector3 scale;                     // スケール
		Vector3 offset;                    // オフセット
		Vector3 colliderCenter;            // コライダーのセンター
		Vector3 colliderSize;              // コライダーのサイズ
		float radius;                      // コライダーの半径
	};

	void Initialize() override;

	void Update() override;

	void Draw(const Camera& camera) override;

	void DrawUI() override;

	void OnCollision(GameObject* gameObject) override;

	void OnCollisionEnter(GameObject* gameObject) override;

	void OnCollisionExit(GameObject* gameObject) override;

	void SetBoxWarningObjectSettings(const BoxWarningObjectSettings& warningObjectSettings) { warningObjectSettings_ = warningObjectSettings; };

private:
	BoxWarningObjectSettings warningObjectSettings_{};
};

