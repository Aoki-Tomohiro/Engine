/**
 * @file ModelComponent.h
 * @brief モデルのコンポーネント
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/3D/Model/Model.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Components/Base/RenderComponent.h"

class ModelComponent : public RenderComponent
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ModelComponent();

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
	/// <param name="camera">カメラ</param>
	void Draw(const Camera& camera) override;

	//モデルを取得・設定
	Model* GetModel() const { return model_; };
	void SetModel(Model* model) { model_->Release(); model_ = model; };

private:
	Model* model_ = nullptr;
};

