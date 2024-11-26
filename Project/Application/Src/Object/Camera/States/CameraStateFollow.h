#pragma once
#include "Engine/Components/Input/Input.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"

/// <summary>
/// 追従カメラの状態
/// </summary>
class CameraStateFollow : public ICameraState
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

private:
	/// <summary>
	/// カメラのX軸の角度を制限
	/// </summary>
	/// <param name="inputValue">入力値</param>
	/// <param name="direction">カメラから追従対象のベクトル</param>
	void ClampVerticalRotation(Vector3& inputValue, const Vector3& direction);

	/// <summary>
	/// カメラの回転を適用
	/// </summary>
	/// <param name="inputValue">入力値</param>
	void ApplyCameraRotation(const Vector3& inputValue);

private:
	//インプット
	Input* input_ = nullptr;
};

