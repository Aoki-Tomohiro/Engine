/**
 * @file BreakableObject.h
 * @brief 破壊オブジェクトを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Application/Src/Object/Editors/ParticleEffectEditor/ParticleEffectEditor.h"

class BreakableObject : public GameObject
{
public:
    /// <summary>
    /// 初期化
    /// </summary>
    void Initialize() override;

    /// <summary>
    /// 更新
    /// </summary>
    void Update() override;

    /// <summary>
    /// 衝突処理
    /// </summary>
    /// <param name="gameObject">衝突相手</param>
    void OnCollision(GameObject* gameObject) override;

    //パーティクルエフェクトエディターを設定
    void SetParticleEffectEditor(ParticleEffectEditor* particleEffectEditor) { particleEffectEditor_ = particleEffectEditor; };

private:
    /// <summary>
    /// 破壊処理
    /// </summary>
    void HandleDestruction();

private:
    //オーディオ
    Audio* audio_ = nullptr;

    //オーディオハンドル
    uint32_t audioHandle_ = 0;

    //トランスフォーム
    TransformComponent* transform_ = nullptr;

    //パーティクルエフェクトエディター
    ParticleEffectEditor* particleEffectEditor_ = nullptr;

    //体力
    float hp_ = 3.0f;
};

