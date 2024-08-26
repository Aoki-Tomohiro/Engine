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
        //トランスフォームを取得
        TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();

        //ブレンドしたアニメーションを適用
        currentAnimation->ApplyBlendAnimation(modelComponent->GetModel(), transformComponent->worldTransform_, nextAnimation, blendFactor_);

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
        //アニメーションの再生
        it->second->PlayAnimation(speed, loop);

        //アニメーションブレンドをしない場合
        if (!isBlending_)
        {
            //現在のアニメーションを更新
            currentAnimation_ = animationName;
            //次のアニメーションをクリアしておく
            nextAnimation_.clear();
        }
        //アニメーションブレンドをする場合
        else
        {
            //現在にアニメーションが無ければ現在のアニメーションを更新
            if (currentAnimation_.empty())
            {
                currentAnimation_ = animationName;
            }
            //現在のアニメーションが存在する場合
            else
            {
                //次のアニメーションが存在する場合は現在のアニメーションを更新
                if (!nextAnimation_.empty())
                {
                    currentAnimation_ = nextAnimation_;
                }

                //次のアニメーションを設定
                nextAnimation_ = animationName;

                //ブレンドファクターを初期化
                blendFactor_ = 0.0f;
            }

        }
    }
}

void AnimatorComponent::StopAnimation()
{
    //再生中のアニメーションを停止
    auto currentAnimation = animations_.find(currentAnimation_);
    if (currentAnimation != animations_.end())
    {
        currentAnimation->second->StopAnimation();
    }

    //次のアニメーションを停止
    auto nextAnimation = animations_.find(nextAnimation_);
    if (nextAnimation != animations_.end())
    {
        nextAnimation->second->StopAnimation();
    }
}

const bool AnimatorComponent::GetIsAnimationFinished(const std::string& animationName) const
{
    Animation* animation = GetAnimation(animationName);
    return animation ? animation->GetIsAnimationFinished() : true;
}

const bool AnimatorComponent::GetIsAnimationFinished()
{
    //現在のアニメーションが設定されていない場合
    if (currentAnimation_.empty())
    {
        //アニメーションがないので終了したと見なす
        return true;
    }

    //アニメーションが終了したかどうかを返す
    return GetIsAnimationFinished(currentAnimation_);
}

const float AnimatorComponent::GetAnimationDuration(const std::string& animationName) const
{
    Animation* animation = GetAnimation(animationName);
    return animation ? animation->GetDuration() : 0.0f;
}

const float AnimatorComponent::GetAnimationDuration()
{
    //現在のアニメーションが設定されていない場合
    if (currentAnimation_.empty())
    {
        //アニメーションがないので0.0fを返す
        return 0.0f;
    }

    //アニメーションの終了時間を返す
    return GetAnimationDuration(currentAnimation_);
}

const float AnimatorComponent::GetAnimationTime(const std::string& animationName) const
{
    Animation* animation = GetAnimation(animationName);
    return animation ? animation->GetAnimationTime() : 0.0f;
}

void AnimatorComponent::SetAnimationTime(const std::string& animationName, const float animationTime)
{
    Animation* animation = GetAnimation(animationName);
    if (animation)
    {
        animation->SetAnimationTime(animationTime);
    }
}

const float AnimatorComponent::GetAnimationTime() const
{
    //現在のアニメーションが設定されていない場合
    if (currentAnimation_.empty())
    {
        //アニメーションがないので0.0fを返す
        return 0.0f;
    }

    //現在のアニメーションの時間を返す
    return GetAnimationTime(currentAnimation_);
}

void AnimatorComponent::SetAnimationTime(const float animationTime)
{
    //現在のアニメーションが設定されている場合
    if (!currentAnimation_.empty())
    {
        //現在のアニメーションの時間を設定
        SetAnimationTime(currentAnimation_, animationTime);
    }
}

Animation* AnimatorComponent::GetAnimation(const std::string& animationName) const
{
    auto it = animations_.find(animationName);
    if (it != animations_.end())
    {
        return it->second.get();
    }
    return nullptr;
}