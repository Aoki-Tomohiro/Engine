#pragma once

class CameraController;

class ICameraState
{
public:
	virtual void Initialize() = 0;

	virtual void Update() = 0;

	void SetCameraController(CameraController* cameraController) { cameraController_ = cameraController; };

protected:
	CameraController* cameraController_ = nullptr;
};