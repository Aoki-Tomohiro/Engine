#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Application/Src/Object/Character/ICharacter.h"

class Enemy : public ICharacter
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
    /// UIの描画
    /// </summary>
    void DrawUI() override;

private:
    /// <summary>
    /// アニメーターの初期化
    /// </summary>
    void InitializeAnimator() override;

    /// <summary>
    /// UIのスプライトの初期化
    /// </summary>
    void InitializeUISprites() override;
};

