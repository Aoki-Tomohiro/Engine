#include "EditorManager.h"

void EditorManager::Initialize()
{
	//パーティクルエフェクトエディターの初期化
	particleEffectEditor_ = std::make_unique<ParticleEffectEditor>();
	particleEffectEditor_->Initialize();

	//コンバットアニメーションエディターの初期化
	combatAnimationEditor_ = std::make_unique<CombatAnimationEditor>();
	combatAnimationEditor_->Initialize();
	combatAnimationEditor_->SetParticleEffectEditor(particleEffectEditor_.get());

	//カメラアニメーションエディターの初期化
	cameraAnimationEditor_ = std::make_unique<CameraAnimationEditor>();
	cameraAnimationEditor_->Initialize();
}

void EditorManager::Update()
{
	//ImGuiウィンドウの開始
	ImGui::Begin("EditorManager");

	//タブバーを開始
	if (ImGui::BeginTabBar("Editors"))
	{
		//コンバットアニメーションエディター
		if (ImGui::BeginTabItem("CombatAnimationEditor"))
		{
			combatAnimationEditor_->Update();
			ImGui::EndTabItem();
		}

		//カメラアニメーションエディター
		if (ImGui::BeginTabItem("CameraAnimationEditor"))
		{
			cameraAnimationEditor_->Update();
			ImGui::EndTabItem();
		}

		//パーティクルエフェクトエディター
		if (ImGui::BeginTabItem("ParticleEffectEditor"))
		{
			particleEffectEditor_->Update();
			ImGui::EndTabItem();
		}

		//タブバーを終了
		ImGui::EndTabBar();
	}

	ImGui::End();
}