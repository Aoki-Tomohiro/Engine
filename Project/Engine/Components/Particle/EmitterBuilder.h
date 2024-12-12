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
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetEmitterName(const std::string& name);

	/// <summary>
	/// パーティクルの生成数を設定
	/// </summary>
	/// <param name="count">パーティクルの生成数</param>
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetCount(const uint32_t count);

	/// <summary>
	/// パーティクルの発生間隔を設定
	/// </summary>
	/// <param name="frequency">発生間隔</param>
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetFrequency(const float frequency);

	/// <summary>
	/// エミッターの寿命を設定
	/// </summary>
	/// <param name="emitterLifeTime">エミッターの寿命</param>
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetEmitterLifeTime(const float emitterLifeTime);

	/// <summary>
	/// 追従対象を設定
	/// </summary>
	/// <param name="followTarget">追従対象</param>
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetFollowTarget(const Vector3* followTarget);

	/// <summary>
	/// 座標を設定
	/// </summary>
	/// <param name="translate">座標</param>
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetTranslation(const Vector3& translate);

	/// <summary>
	/// 半径を設定
	/// </summary>
	/// <param name="radius">半径</param>
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetRadius(const float radius);

	/// <summary>
	/// 回転を設定
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetRotate(const Vector3& min, const Vector3& max);

	/// <summary>
	/// スケールを設定
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	/// <returns>自分のポインタ</returns>
	EmitterBuilder& SetScale(const Vector3& min, const Vector3& max);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <returns></returns>
	EmitterBuilder& SetVelocity(const Vector3& min, const Vector3& max);

	EmitterBuilder& SetLifeTime(const float min, const float max);

	EmitterBuilder& SetColor(const Vector4& min, const Vector4& max);

	EmitterBuilder& SetAlignToDirection(const bool alignToDirection);

	EmitterBuilder& SetEnableColorOverLifeTime(const bool enableColorOverLifeTime);

	EmitterBuilder& SetTargetColor(const Vector3& targetColor);

	EmitterBuilder& SetEnableAlphaOverLifeTime(const bool enableAlphaOverLifeTime);

	EmitterBuilder& SetTargetAlpha(const float targetAlpha);

	EmitterBuilder& SetEnableSizeOverLifeTime(const bool enableSizeOverLifeTime);

	EmitterBuilder& SetTargetScale(const Vector3& targetScale);

	EmitterBuilder& SetEnableRotationOverLifeTime(const bool enableRotationOverLifeTime);

	EmitterBuilder& SetRotSpeed(const Vector3& rotSpeed);

	EmitterBuilder& SetIsBillboard(const bool isBillboard);

	ParticleEmitter* Build();

private:
	//エミッター
	ParticleEmitter* particleEmitter_ = nullptr;
};

