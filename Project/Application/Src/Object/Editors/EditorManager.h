#pragma once
#include "Application/Src/Object/Editors/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/Editors/CameraAnimationEditor/CameraAnimationEditor.h"
#include "Application/Src/Object/Editors/ParticleEffectEditor/ParticleEffectEditor.h"

/// <summary>
/// エディターを管理するクラス
/// </summary>
class EditorManager
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	//コンバットアニメーションエディターを取得
	CombatAnimationEditor* GetCombatAnimationEditor() const { return combatAnimationEditor_.get(); };

	//カメラアニメーションエディターを取得
	CameraAnimationEditor* GetCameraAnimationEditor() const { return cameraAnimationEditor_.get(); };

	//パーティクルエフェクトエディターを取得
	ParticleEffectEditor* GetParticleEffectEditor() const { return particleEffectEditor_.get(); };

private:
	//コンバットアニメーションエディター
	std::unique_ptr<CombatAnimationEditor> combatAnimationEditor_ = nullptr;

	//カメラアニメーションエディター
	std::unique_ptr<CameraAnimationEditor> cameraAnimationEditor_ = nullptr;

	//パーティクルエフェクトエディター
	std::unique_ptr<ParticleEffectEditor> particleEffectEditor_ = nullptr;
};

