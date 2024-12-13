/**
 * @file Camera.h
 * @brief カメラを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/Application.h"
#include "Engine/Base/UploadBuffer.h"
#include "Engine/Base/ConstantBuffers.h"
#include <memory>

class Camera
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// ビュー行列の更新
	/// </summary>
	void UpdateViewMatrix();

	/// <summary>
	/// プロジェクション行列の更新
	/// </summary>
	void UpdateProjectionMatrix();

	/// <summary>
	/// 行列を更新
	/// </summary>
	void UpdateMatrix();

	/// <summary>
	/// 行列を書き込む
	/// </summary>
	void TransferMatrix();

	//コンスタントバッファを取得
	UploadBuffer* GetConstantBuffer() const { return constBuff_.get(); };

	//カメラをコピー
	Camera& operator=(const Camera& rhs)
	{
		if (this != &rhs)
		{
			rotation_ = rhs.rotation_;
			quaternion_ = rhs.quaternion_;
			translation_ = rhs.translation_;
			matView_ = rhs.matView_;
			matProjection_ = rhs.matProjection_;
			fov_ = rhs.fov_;
			aspectRatio_ = rhs.aspectRatio_;
			nearClip_ = rhs.nearClip_;
			farClip_ = rhs.farClip_;
			rotationType_ = rhs.rotationType_;
		}
		return *this;
	}

private:
	std::unique_ptr<UploadBuffer> constBuff_ = nullptr;

public:
	Vector3 rotation_ = { 0.0f,0.0f,0.0f };

	Quaternion quaternion_ = { 0.0f,0.0f,0.0f,1.0f };

	Vector3 translation_ = { 0.0f,0.0f,-50.0f };

	Matrix4x4 matView_{};

	Matrix4x4 matProjection_{};

	float fov_ = 45.0f * 3.141592654f / 180.0f;

	float aspectRatio_ = static_cast<float>(Application::kClientWidth) / static_cast<float>(Application::kClientHeight);

	float nearClip_ = 0.1f;

	float farClip_ = 1000.0f;

	RotationType rotationType_ = RotationType::Euler;
};

