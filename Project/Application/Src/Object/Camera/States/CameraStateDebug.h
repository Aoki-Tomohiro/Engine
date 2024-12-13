/**
 * @file CameraStateDebug.h
 * @brief カメラのデバッグ状態を管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include "Application/Src/Object/Editors/CameraAnimationEditor/CameraPath.h"

class CameraStateDebug : public ICameraState
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize() override {};

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;
};

