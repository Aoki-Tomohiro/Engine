/**
 * @file CameraManager.h
 * @brief 全てのカメラを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/3D/Camera/Camera.h"
#include <string>
#include <map>

class CameraManager
{
public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static CameraManager* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// カメラを生成
	/// </summary>
	/// <param name="cameraName">カメラの名前</param>
	/// <returns>カメラ</returns>
	static Camera* CreateCamera(const std::string& cameraName);

	/// <summary>
	/// ガメラを取得
	/// </summary>
	/// <param name="cameraName">名前</param>
	/// <returns>指定したカメラ</returns>
	static Camera* GetCamera(const std::string& cameraName);

private:
	CameraManager() = default;
	~CameraManager() = default;
	CameraManager(const CameraManager&) = delete;
	const CameraManager& operator=(const CameraManager&) = delete;

	/// <summary>
	/// カメラを内部で生成
	/// </summary>
	/// <param name="cameraName">カメラの名前</param>
	/// <returns>カメラ</returns>
	Camera* CreateCameraInternal(const std::string& cameraName);

	/// <summary>
	/// カメラを内部で取得
	/// </summary>
	/// <param name="cameraName">カメラの名前</param>
	/// <returns>カメラ</returns>
	Camera* GetCameraInternal(const std::string& cameraName);

private:
	static CameraManager* instance_;

	std::map<std::string, std::unique_ptr<Camera>> cameras_{};
};

