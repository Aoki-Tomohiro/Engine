#include "GameTitleScene.h"
#include "Engine/Framework/Scene/SceneManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/LevelLoader/LevelLoader.h"
#include <numbers>

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();

	//GameObjectManagerのインスタンスを取得
	gameObjectManager_ = GameObjectManager::GetInstance();

	//GameObjectのクリア
	gameObjectManager_->Clear();

	//LevelDataの読み込み
	LevelLoader::Load("TitleScene");

	//カメラを初期化
    camera_ = gameObjectManager_->GetCamera();
	camera_->rotation_.x = 0.3f;

	//トランジションの生成
	transition_ = std::make_unique<Transition>();
	transition_->Initialize();
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update()
{
	//GameObjectManagerの更新
	gameObjectManager_->Update();

	//トランジションの更新
	transition_->Update();

	//カメラの更新
	CameraUpdate();

	//フェードイン処理
	HandleTransition();

	//ImGui
	ImGui::Begin("GameTitleScene");
	ImGui::Text("Space or AButton: GamePlayScene");
	ImGui::DragFloat3("CameraRotation", &camera_->rotation_.x);
	ImGui::End();
}

void GameTitleScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//GameObjectManagerの描画
	gameObjectManager_->Draw();

	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion
}

void GameTitleScene::DrawUI()
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//トランジションの描画
	transition_->Draw();

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}

void GameTitleScene::CameraUpdate()
{
	//回転速度
	const float kRotSpeed = 0.006f;

	//回転処理
	camera_->rotation_.y += kRotSpeed;
	camera_->rotation_.y = std::fmod(camera_->rotation_.y, std::numbers::pi_v<float> * 2.0f);

	//回転行列の作成
	Matrix4x4 rotateYMatrix = Mathf::MakeRotateYMatrix(camera_->rotation_.y);

	//Offsetに回転行列を適用
	Vector3 offset = { 0.0f,30.0f ,-80.0f };
	offset = Mathf::TransformNormal(offset, rotateYMatrix);

	//カメラの座標を設定
	camera_->translation_ = offset;

	//カメラの更新
	camera_->UpdateMatrix();
}

void GameTitleScene::HandleTransition()
{
	//SpaceキーかAボタンを押したらフェードインさせる
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		transition_->SetFadeState(Transition::FadeState::In);
	}
	else if (input_->IsPushKeyEnter(DIK_SPACE))
	{
		transition_->SetFadeState(Transition::FadeState::In);
	}

	//シーン遷移
	if (transition_->GetFadeInComplete())
	{
		sceneManager_->ChangeScene("GamePlayScene");
	}
}