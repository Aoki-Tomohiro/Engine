#pragma once
#include "Engine/Framework/IGameObject.h"

class Ground : public IGameObject 
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
	/// 地面の表面のモデルを設定
	/// </summary>
	/// <param name="model"></param>
	void SetGroundSurfaceModel(Model* model) { groundSurfaceModel_ = model; };

private:
	//地面の表面のモデル
	Model* groundSurfaceModel_ = nullptr;

	//地面の表面のワールドトランスフォーム
	WorldTransform groundSurfaceWorldTransform_{};
};

