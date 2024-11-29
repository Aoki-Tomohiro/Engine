#pragma once
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include "Application/Src/Object/Editors/CameraAnimationEditor/CameraPath.h"
#include <string>

/// <summary>
/// アニメーション状態
/// </summary>
class CameraStateAnimation : public ICameraState
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	/// <param name="animationName">アニメーションの名前</param>
	/// <param name="animationSpeed">アニメーションの速度</param>
	/// <param name="syncWithCharacterAnimation">キャラクターのアニメーションに同期するかどうか</param>
	CameraStateAnimation(const std::string& animationName, const float animationSpeed, const bool syncWithCharacterAnimation) : animationName_(animationName), animationSpeed_(animationSpeed), syncWithCharacterAnimation_(syncWithCharacterAnimation) {};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override;

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

private:
	/// <summary>
	/// アニメーションの時間の更新
	/// </summary>
	void UpdateAnimationTime();

	/// <summary>
	/// カメラのトランスフォームの更新
	/// </summary>
	/// <param name="keyFrame">キーフレーム</param>
	void UpdateCameraTransform(const CameraPath::CameraKeyFrame& keyFrame);

private:
	//カメラパス
	CameraPath cameraPath_{};

	//アニメーションの名前
	std::string animationName_{};

	//アニメーションの速度
	float animationSpeed_ = 1.0f;

	//プレイヤーのアニメーションと同期するかどうか
	bool syncWithCharacterAnimation_ = false;

	//アニメーションの時間
	float animationTime_ = 0.0f;

	//追従対象のクォータニオン
	Quaternion followTargetQuaternion_ = Mathf::IdentityQuaternion();
};

