#include "GameCore.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Engine/Utilities/RandomGenerator.h"

void GameCore::Initialize() 
{
	//Applicationの初期化
	application_ = Application::GetInstance();
	application_->CreateGameWindow(L"LE2A_01_アオキ_トモヒロ", Application::kClientWidth, Application::kClientHeight);

	//GraphicsCoreの初期化
	graphicsCore_ = GraphicsCore::GetInstance();
	graphicsCore_->Initialize();

	//TextureManagerの初期化
	textureManager_ = TextureManager::GetInstance();
	textureManager_->Initialize();

	//ImGuiManagerの初期化
	imguiManager_ = ImGuiManager::GetInstance();
	imguiManager_->Initialize();

	//Rendererの初期化
	renderer_ = Renderer::GetInstance();
	renderer_->Initialize();

	//Inputの初期化
	input_ = Input::GetInstance();
	input_->Initialize();

	//Audioの初期化
	audio_ = Audio::GetInstance();
	audio_->Initialize();

	//PostProcessの初期化
	postProcess_ = PostProcess::GetInstance();
	postProcess_->Initialize();

	//SceneManagerのインスタンスを取得
	sceneManager_ = SceneManager::GetInstance();

	//ランダムエンジンの初期化
	RandomGenerator::Initialize();
}

void GameCore::Finalize() 
{
	//GameObjectManagerの解放
	GameObjectManager::Destroy();

	//PostProcessの開放
	PostProcess::Destroy();

	//ParticmeManagerの開放
	ParticleManager::Destroy();

	//Audioの開放
	audio_->Finalize();

	//Inputの開放
	Input::Destroy();

	//SceneManagerの開放
	SceneManager::Destroy();

	//ModelManagerの開放
	ModelManager::Destroy();

	//Rendererの開放
	Renderer::Destroy();

	//ImGuiManagerの開放
	imguiManager_->ShutDown();

	//TextureManagerの開放
	TextureManager::Destroy();

	//GraphicsCoreの解放
	GraphicsCore::Destroy();

	//ゲームウィンドウを閉じる
	application_->CloseGameWindow();
}

void GameCore::Update()
{
	//Inputの更新
	input_->Update();

	//ImGui受付開始
	imguiManager_->Begin();

	//GlobalVariablesの更新
	GlobalVariables::GetInstance()->Update();

	//SceneManagerの更新
	sceneManager_->Update();

	//PostProcessの更新
	postProcess_->Update();

	//ImGui受付終了
	imguiManager_->End();
}

void GameCore::Draw()
{
	//シーンの描画前処理
	renderer_->PreDraw();

	//SceneManagerの描画
	sceneManager_->Draw();

	//シーンの描画後処理
	renderer_->PostDraw();

	//ポストプロセスの適用
	postProcess_->ApplyPostProcess();

	//描画前処理
	graphicsCore_->PreDraw();

	//PostProcessの描画
	postProcess_->Draw();

	//SceneManagerのスプライト描画
	sceneManager_->DrawUI();

	//ImGuiの描画
	imguiManager_->Draw();

	//描画後処理
	graphicsCore_->PostDraw();
}

bool GameCore::IsEndRequest() 
{
	bool endRequest = application_->ProcessMessage();
	return endRequest;
}

void GameCore::Run()
{
	//初期化
	Initialize();

	//ゲームループ
	while (true) 
	{
		//ゲームループを抜ける
		if (IsEndRequest()) 
		{
			break;
		}

		//更新
		Update();

		//描画
		Draw();
	}

	//終了
	Finalize();
}