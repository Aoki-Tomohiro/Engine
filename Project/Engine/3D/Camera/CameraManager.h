#pragma once
#include "Engine/3D/Camera/Camera.h"
#include <string>
#include <map>

class CameraManager
{
public:
	static CameraManager* GetInstance();

	static void Destroy();

	static Camera* CreateCamera(const std::string& cameraName);

	static Camera* GetCamera(const std::string& cameraName);

private:
	CameraManager() = default;
	~CameraManager() = default;
	CameraManager(const CameraManager&) = delete;
	const CameraManager& operator=(const CameraManager&) = delete;

	Camera* CreateCameraInternal(const std::string& cameraName);

	Camera* GetCameraInternal(const std::string& cameraName);

private:
	static CameraManager* instance_;

	std::map<std::string, std::unique_ptr<Camera>> cameras_{};
};

