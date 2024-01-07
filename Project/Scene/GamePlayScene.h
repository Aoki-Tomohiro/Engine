#pragma once
#include "Engine/Framework/IScene.h"
#include "Engine/Framework/GameObjectManager.h"
#include "Engine/Base/Renderer.h"
#include "Engine/3D/ModelManager.h"
#include "Engine/Components/Input.h"
#include "Engine/Components/CollisionManager.h"

#include "Project/Object/Player/Player.h"
#include "Project/Object/FollowCamera/FollowCamera.h"
#include "Project/Object/Ground/Ground.h"
#include "Project/Object/Skydome/Skydome.h"
#include "Project/Object/Boss/Boss.h"
#include "Project/Object/LockOn/LockOn.h"

class GamePlayScene : public IScene
{
public:
	static const uint32_t kStopTime = 2;
	static const uint32_t kShakeTime = 20;

	void Initialize() override;

	void Finalize() override;

	void Update() override;

	void Draw() override;

	void DrawUI() override;

	void UpdateImGui();

private:
	Input* input_ = nullptr;

	Renderer* renderer_ = nullptr;

	GameObjectManager* gameObjectManager_ = nullptr;

	std::unique_ptr<CollisionManager> collisionManager_ = nullptr;

	//カメラ関連
	Camera camera_{};
	bool cameraShakeEnable_ = false;
	uint32_t shakeTimer_ = 0;

	//プレイヤー
	Model* playerModel_ = nullptr;
	Player* player_ = nullptr;

	//ロックオン
	std::unique_ptr<LockOn> lockOn_ = nullptr;

	//追従カメラ
	std::unique_ptr<FollowCamera> followCamera_ = nullptr;

	//地面
	Model* groundSurfaceModel_ = nullptr;
	Model* groundModel_ = nullptr;
	Ground* ground_ = nullptr;

	//天球
	Model* skydomeModel_ = nullptr;
	Skydome* skydome_ = nullptr;

	//ボス
	Model* bossModel_ = nullptr;
	Boss* boss_ = nullptr;

	//ヒットストップ関連
	bool isStop_ = false;
	int32_t stopTimer_ = 0;
};

