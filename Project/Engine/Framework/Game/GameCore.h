/**
 * @file GameCore.h
 * @brief ゲームの基底コアクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Base/Application.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Camera/CameraManager.h"
#include "Engine/3D/Primitive/LineRenderer.h"
#include "Engine/3D/Primitive/TrailRenderer.h"
#include "Engine/3D/Lights/LightManager.h"
#include "Engine/Utilities/D3DResourceLeakChecker.h"
#include <condition_variable>
#include <mutex>

class GameCore
{
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~GameCore() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();

	/// <summary>
	/// 更新
	/// </summary>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw();

	/// <summary>
	/// ゲームの終了させるかどうか
	/// </summary>
	/// <returns>ゲームの終了させるかどうか</returns>
	virtual bool IsEndRequest();

	/// <summary>
	/// メインループ
	/// </summary>
	void Run();

protected:
	D3DResourceLeakChecker leakChecker{};

	Application* application_ = nullptr;

	GraphicsCore* graphicsCore_ = nullptr;

	TextureManager* textureManager_ = nullptr;

	ImGuiManager* imguiManager_ = nullptr;

	Renderer* renderer_ = nullptr;

	Input* input_ = nullptr;

	Audio* audio_ = nullptr;

	ParticleManager* particleManager_ = nullptr;

	PostEffects* postEffects_ = nullptr;

	CollisionAttributeManager* collisionAttributeManager_ = nullptr;

	SceneManager* sceneManager_ = nullptr;

	GameObjectManager* gameObjectManager_ = nullptr;

	LineRenderer* lineRenderer_ = nullptr;

	TrailRenderer* trailRenderer_ = nullptr;

	LightManager* lightManager_ = nullptr;

	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;

	std::unique_ptr<AbstractGameObjectFactory> gameObjectFactory_ = nullptr;

	std::mutex mutex;

	std::condition_variable condition;

	bool isLoading_ = false;
};

