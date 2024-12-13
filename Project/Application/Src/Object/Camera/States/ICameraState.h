/**
 * @file ICameraState.h
 * @brief カメラの状態の基底クラスを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once

class CameraController;

class ICameraState
{
public:
	virtual ~ICameraState() = default;

	virtual void Initialize() = 0;

	virtual void Update() = 0;

	void SetCameraController(CameraController* cameraController) { cameraController_ = cameraController; };

protected:
	CameraController* cameraController_ = nullptr;
};