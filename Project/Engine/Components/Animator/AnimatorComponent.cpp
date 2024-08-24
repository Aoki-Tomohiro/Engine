#include "AnimatorComponent.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Utilities/GameTimer.h"

void AnimatorComponent::Initialize()
{

}

void AnimatorComponent::Update()
{
    //現在のアニメーションがない場合は何もしない
    if (currentAnimation_.empty())
    {
        return;
    }

    //現在のアニメーションを取得
    Animation* currentAnimation = animations_[currentAnimation_].get();
    if (!currentAnimation)
    {
        return;
    }

    //モデルを取得
    ModelComponent* modelComponent = owner_->GetComponent<ModelComponent>();
    if (!modelComponent)
    {
        return;
    }

    //次のアニメーションを取得
    Animation* nextAnimation = !nextAnimation_.empty() ? animations_[nextAnimation_].get() : nullptr;
    //次のアニメーションがある場合
    if (nextAnimation && isBlending_)
    {
        //ブレンドファクターを更新
        blendFactor_ += GameTimer::GetDeltaTime() / blendDuration_;
        blendFactor_ = std::min<float>(blendFactor_, 1.0f); //1.0f を超えないようにする

        //完全に次のアニメーションに切り替わったら、現在のアニメーションを更新
        if (blendFactor_ >= 1.0f)
        {
            currentAnimation_ = nextAnimation_;
            nextAnimation_.clear();
            blendFactor_ = 0.0f;
        }

        //トランスフォームを取得
        TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();

        //ブレンドしたアニメーションを適用
        currentAnimation->ApplyBlendAnimation(modelComponent->GetModel(), transformComponent->worldTransform_, nextAnimation, blendFactor_);
    }
    else
    {
        //トランスフォームを取得
        TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();

        //アニメーションの適用
        currentAnimation->ApplyAnimation(modelComponent->GetModel(), transformComponent->worldTransform_);
    }

    //モデルの更新
    modelComponent->Update();
}

void AnimatorComponent::AddAnimation(const std::string& animationName, Animation* animation)
{
    //アニメーションをマップに追加
    animations_[animationName] = std::unique_ptr<Animation>(animation);
}

void AnimatorComponent::PlayAnimation(const std::string& animationName, const float speed, const bool loop)
{
    //アニメーションが存在するかチェック
    auto it = animations_.find(animationName);
    if (it != animations_.end())
    {
        //アニメーションの再生速度とループ設定を行う
        it->second->SetAnimationSpeed(speed);
        it->second->SetLoop(loop);

        //次のアニメーションとして設定
        nextAnimation_ = animationName;

        //ブレンドの準備をする
        blendFactor_ = 0.0f;
    }
}

void AnimatorComponent::StopAnimation()
{
    //再生中のアニメーションをクリア
    currentAnimation_.clear();
    nextAnimation_.clear();
}