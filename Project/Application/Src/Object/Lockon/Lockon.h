/**
 * @file Lockon.h
 * @brief ロックオン処理を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/2D/Sprite.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Base/Application.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Math/MathFunction.h"

class Lockon
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Update(const Camera* camera);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// ロックオンターゲットの座標を取得
	/// </summary>
	/// <returns>ロックオンターゲットの座標</returns>
	const Vector3 GetTargetPosition() const;

	/// <summary>
	/// ロックオンされているかどうか
	/// </summary>
	/// <returns>ロックオンされているかどうか</returns>
	bool ExistTarget() const { return target_ ? true : false; };

private:
	/// <summary>
	/// ロックオンの更新
	/// </summary>
	void UpdateTargeting();

	/// <summary>
	/// ロックオンターゲットの座標をワールド座標からスクリーン座標に変換
	/// </summary>
	/// <param name="worldPosition">ロックオンターゲットの座標</param>
	/// <param name="camera">カメラ</param>
	/// <returns>スクリーン座標</returns>
	const Vector2 WorldToScreenPosition(const Vector3& worldPosition, const Camera* camera) const;

	/// <summary>
	/// ロックオンのスプライトに座標を設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetLockonMarkPosition(const Camera* camera);

private:
	//インプット
	Input* input_ = nullptr;

	//ロックオンのスプライト
	std::unique_ptr<Sprite> lockonMark_ = nullptr;

	//ロックオンターゲット
	const TransformComponent* target_ = nullptr;

	//ロックオンスプライトのスケール
	Vector2 lockonSpriteScale_ = { 0.4f,0.4f };
};

