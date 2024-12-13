/**
 * @file EmitterBuilder.h
 * @brief エミッターを生成するBuilder
 * @author 青木智滉
 * @date
 */

#pragma once
#include "ParticleEmitter.h"

class EmitterBuilder
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	EmitterBuilder();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~EmitterBuilder();

	/// <summary>
	/// エミッターの名前を設定
	/// </summary>
	/// <param name="name">エミッターの名前</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetEmitterName(const std::string& name);

	/// <summary>
	/// パーティクルの生成数を設定
	/// </summary>
	/// <param name="count">パーティクルの生成数</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetCount(const uint32_t count);

	/// <summary>
	/// パーティクルの発生間隔を設定
	/// </summary>
	/// <param name="frequency">発生間隔</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetFrequency(const float frequency);

	/// <summary>
	/// エミッターの寿命を設定
	/// </summary>
	/// <param name="emitterLifeTime">エミッターの寿命</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetEmitterLifeTime(const float emitterLifeTime);

	/// <summary>
	/// 追従対象を設定
	/// </summary>
	/// <param name="followTarget">追従対象</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetFollowTarget(const Vector3* followTarget);

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="translate">座標</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetTranslation(const Vector3& translate);

	/// <summary>
	/// 半径を設定
	/// </summary>
	/// <param name="radius">半径</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetRadius(const float radius);

	/// <summary>
	/// 回転を設定
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetRotate(const Vector3& min, const Vector3& max);

	/// <summary>
	/// スケールを設定
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetScale(const Vector3& min, const Vector3& max);

	/// <summary>
	/// 速度を設定
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetVelocity(const Vector3& min, const Vector3& max);

	/// <summary>
	/// パーティクルの寿命を設定
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetLifeTime(const float min, const float max);

	/// <summary>
	/// 色を設定
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetColor(const Vector4& min, const Vector4& max);

	/// <summary>
	/// 進行方向に回転させるかを設定
	/// </summary>
	/// <param name="alignToDirection">進行方向に回転させるか</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetAlignToDirection(const bool alignToDirection);

	/// <summary>
	/// 寿命に基づいて色を変えるかを設定
	/// </summary>
	/// <param name="enableColorOverLifeTime">寿命に基づいて色を変えるか</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetEnableColorOverLifeTime(const bool enableColorOverLifeTime);

	/// <summary>
	/// 目標の色を設定
	/// </summary>
	/// <param name="targetColor">目標の色</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetTargetColor(const Vector3& targetColor);

	/// <summary>
	/// 寿命に応じて透明度を変えるかを設定
	/// </summary>
	/// <param name="enableAlphaOverLifeTime">寿命に応じて透明度を変えるか</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetEnableAlphaOverLifeTime(const bool enableAlphaOverLifeTime);

	/// <summary>
	/// 目標の透明度を設定
	/// </summary>
	/// <param name="targetAlpha">目標の透明度</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetTargetAlpha(const float targetAlpha);

	/// <summary>
	/// 寿命に応じてサイズを変えるかを設定
	/// </summary>
	/// <param name="enableSizeOverLifeTime">寿命に応じてサイズを変えるか</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetEnableSizeOverLifeTime(const bool enableSizeOverLifeTime);

	/// <summary>
	/// 目標のスケールを設定
	/// </summary>
	/// <param name="targetScale">目標のスケール</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetTargetScale(const Vector3& targetScale);

	/// <summary>
	/// 寿命に応じて回転させるかを設定
	/// </summary>
	/// <param name="enableRotationOverLifeTime">寿命に応じて回転させるか</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetEnableRotationOverLifeTime(const bool enableRotationOverLifeTime);

	/// <summary>
	/// 回転速度を設定
	/// </summary>
	/// <param name="rotSpeed">回転速度</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetRotSpeed(const Vector3& rotSpeed);

	/// <summary>
	/// ビルボードさせるかを設定
	/// </summary>
	/// <param name="isBillboard">ビルボードさせるか</param>
	/// <returns>自分の参照</returns>
	EmitterBuilder& SetIsBillboard(const bool isBillboard);

	/// <summary>
	/// エミッターを生成
	/// </summary>
	/// <returns>エミッター</returns>
	ParticleEmitter* Build();

private:
	//エミッター
	ParticleEmitter* particleEmitter_ = nullptr;
};

