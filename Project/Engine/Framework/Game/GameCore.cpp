#include "GameCore.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Utilities/GameTimer.h"

void GameCore::Initialize()
{
	//Applicationの初期化
	application_ = Application::GetInstance();
	application_->CreateGameWindow(L"DirectXGame", Application::kClientWidth, Application::kClientHeight);

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

	//ParticleManagerの初期化
	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Initialize();

	//PostEffectsの初期化
	postEffects_ = PostEffects::GetInstance();
	postEffects_->Initialize();

	//SceneManagerのインスタンスを取得
	sceneManager_ = SceneManager::GetInstance();

	//LineRendererの初期化
	lineRenderer_ = LineRenderer::GetInstance();
	lineRenderer_->Initialize();

	//LightManagerの初期化
	lightManager_ = LightManager::GetInstance();
	lightManager_->Initialize();

	//RandomGeneratorの初期化
	RandomGenerator::Initialize();
}

void GameCore::Finalize()
{
	//LightManagerの開放
	LightManager::Destroy();

	//LineRendererの開放
	LineRenderer::Destroy();

	//ModelManagerの解放
	ModelManager::Destroy();

	//PostEffectsの解放
	PostEffects::Destroy();

	//ParticleManagerの開放
	ParticleManager::Destroy();

	//Audioの開放
	audio_->Finalize();

	//Inputの開放
	Input::Destroy();

	//GameObjectManagerの開放
	GameObjectManager::Destroy();

	//SceneManagerの開放
	SceneManager::Destroy();

	//Rendererの解放
	Renderer::Destroy();

	//ImGuiの解放
	imguiManager_->ShutDown();

	//TextureManagerの解放
	TextureManager::Destroy();

	//GraphicsCoreの解放
	GraphicsCore::Destroy();

	//ゲームウィンドウを閉じる
	application_->CloseGameWindow();
}

void GameCore::Update()
{
	//GameTimerの更新
	GameTimer::Update();

	//Inputの更新
	input_->Update();

	//ImGui受け付け開始
	imguiManager_->Begin();

	//ロードシーンに切り替え
	if (sceneManager_->GetLoadingScreenVisible())
	{
		isLoading_ = true;
	}

	//SceneManagerの更新
	sceneManager_->Update();

	//Particleの更新
	particleManager_->Update();

	//GlovalVariablesの更新
	GlobalVariables::GetInstance()->Update();

	//LightManagerの更新
	lightManager_->Update();

	//PostEffectsの更新
	postEffects_->Update();

	//ImGui受付終了
	imguiManager_->End();
}

void GameCore::Draw()
{
	//描画前処理
	renderer_->PreDraw();

	//SceneManagerの描画
	sceneManager_->Draw();

	//パーティクルの描画
	particleManager_->Draw();

	//描画後処理
	renderer_->PostDraw();

	//PostEffectsの適用
	postEffects_->Apply();

	//描画前処理
	graphicsCore_->PreDraw();

	//PostEffectsの描画
	postEffects_->Draw();

	//ラインの描画
	lineRenderer_->Draw();

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

	bool exit = false;
	//バックグラウンドループ
	std::thread th([&]() {
		while (!exit)
		{
			std::unique_lock<std::mutex> uniqueLock(mutex);
			condition.wait(uniqueLock, [&]() {return true; });
			if (isLoading_)
			{
				sceneManager_->Load();
				isLoading_ = false;
			}
		}
		});


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
	exit = true;
	th.join();
	Finalize();
}