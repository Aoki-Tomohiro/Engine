#include "CameraManager.h"

//実体定義
CameraManager* CameraManager::instance_ = nullptr;

CameraManager* CameraManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new CameraManager();
	}
	return instance_;
}

void CameraManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

Camera* CameraManager::CreateCamera(const std::string& cameraName)
{
	Camera* newCamera = CameraManager::GetInstance()->CreateCameraInternal(cameraName);
	return newCamera;
}

Camera* CameraManager::GetCamera(const std::string& cameraName)
{
	Camera* camera = CameraManager::GetInstance()->GetCameraInternal(cameraName);
	return camera;
}

Camera* CameraManager::CreateCameraInternal(const std::string& cameraName)
{
	auto it = cameras_.find(cameraName);
	if (it != cameras_.end()) 
	{
		return it->second.get();
	}

	//カメラの生成
	std::unique_ptr<Camera> newCamera = std::make_unique<Camera>();
	newCamera->Initialize();
	cameras_[cameraName] = std::move(newCamera);
	return cameras_[cameraName].get();
}

Camera* CameraManager::GetCameraInternal(const std::string& cameraName)
{
	auto it = cameras_.find(cameraName);
	if (it != cameras_.end())
	{
		return it->second.get();
	}
	return nullptr;
}