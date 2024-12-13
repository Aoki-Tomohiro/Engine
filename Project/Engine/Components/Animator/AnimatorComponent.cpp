/**
 * @file AnimatorComponent.cpp
 * @brief アニメーターのコンポーネント
 * @author 青木智滉
 * @date
 */

#include "AnimatorComponent.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Utilities/GameTimer.h"

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
        currentAnimation->ApplyBlendAnimation(modelComponent->GetModel(), transformComponent->worldTransform_, nextAnimation, blendFactor_, inPlaceAxis_);

        //ブレンドファクターを更新
        blendFactor_ += GameTimer::GetDeltaTime() / blendDuration_;
        blendFactor_ = std::min<float>(blendFactor_, 1.0f); //1.0f を超えないようにする

        //完全に次のアニメーションに切り替わった場合
        if (blendFactor_ >= 1.0f)
        {
            //現在のアニメーションを更新
            currentAnimation_ = nextAnimation_;
            //次のアニメーションをクリア
            nextAnimation_.clear();
            //ブレンドファクターを初期化
            blendFactor_ = 0.0f;
            //アニメーションブレンドの終了フラグを立てる
            isBlendingCompleted_ = true;
        }
        else
        {
            //アニメーションブレンドの終了フラグを解除
            isBlendingCompleted_ = false;
        }
    }
    else
    {
        //トランスフォームを取得
        TransformComponent* transformComponent = owner_->GetComponent<TransformComponent>();

        //アニメーションの適用
        currentAnimation->ApplyAnimation(modelComponent->GetModel(), transformComponent->worldTransform_, inPlaceAxis_);

        //アニメーションブレンドの終了フラグを立てる
        isBlendingCompleted_ = true;
    }

    //モデルの更新
    modelComponent->Update();
}

void AnimatorComponent::AddAnimation(const std::string& animationName, Animation* animation)
{
    //アニメーションをマップに追加
    animations_[animationName] = std::unique_ptr<Animation>(animation);
}

void AnimatorComponent::PlayAnimation(const std::string& animationName, const float speed, const bool loop, const Vector3& inPlaceAxis)
{
    //アニメーションが存在するかチェック
    auto it = animations_.find(animationName);
    if (it != animations_.end())
    {
        //固定する軸を設定
        inPlaceAxis_ = inPlaceAxis;

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

                //アニメーションブレンドの終了フラグを解除
                isBlendingCompleted_ = false;
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

void AnimatorComponent::PauseAnimation()
{
    //再生中のアニメーションを一時停止
    auto currentAnimation = animations_.find(currentAnimation_);
    if (currentAnimation != animations_.end())
    {
        currentAnimation->second->PauseAnimation();
    }

    //次のアニメーションを一時停止
    auto nextAnimation = animations_.find(nextAnimation_);
    if (nextAnimation != animations_.end())
    {
        nextAnimation->second->PauseAnimation();
    }
}

void AnimatorComponent::ResumeAnimation()
{
    //再生中のアニメーションを一時停止を解除
    auto currentAnimation = animations_.find(currentAnimation_);
    if (currentAnimation != animations_.end())
    {
        currentAnimation->second->ResumeAnimation();
    }

    //次のアニメーションを一時停止を解除
    auto nextAnimation = animations_.find(nextAnimation_);
    if (nextAnimation != animations_.end())
    {
        nextAnimation->second->ResumeAnimation();
    }
}

const bool AnimatorComponent::GetIsBlendingCompleted() const
{
    return isBlendingCompleted_;
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

    //アニメーションの名前を取得
    std::string animationName = currentAnimation_;
    //次のアニメーションが設定されていたらそのアニメーションを設定
    if (!nextAnimation_.empty())
    {
        animationName = nextAnimation_;
    }

    //アニメーションが終了したかどうかを返す
    return GetIsAnimationFinished(animationName);
}

const float AnimatorComponent::GetAnimationDuration(const std::string& animationName) const
{
    Animation* animation = GetAnimation(animationName);
    return animation ? animation->GetDuration() : 0.0f;
}

const float AnimatorComponent::GetCurrentAnimationDuration() const
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

const float AnimatorComponent::GetNextAnimationDuration() const
{
    //ブレンド中でかつ次のアニメーションが設定されている場合
    if (isBlending_ && !nextAnimation_.empty())
    {
        //アニメーションの終了時間を返す
        return GetAnimationDuration(nextAnimation_);
    }

    //アニメーションがないので0.0fを返す
    return 0.0f;
}

const float AnimatorComponent::GetAnimationTime(const std::string& animationName) const
{
    //指定された名前のアニメーションを取得
    Animation* animation = GetAnimation(animationName);
    //アニメーションが存在する場合、そのアニメーションの時間を返す
    //アニメーションが存在しない場合は0.0fを返す
    return animation ? animation->GetAnimationTime() : 0.0f;
}

void AnimatorComponent::SetAnimationTime(const std::string& animationName, const float animationTime)
{
    //指定された名前のアニメーションを取得
    Animation* animation = GetAnimation(animationName);
    //アニメーションが存在する場合、そのアニメーションの時間を設定
    if (animation)
    {
        animation->SetAnimationTime(animationTime);
    }
}

const float AnimatorComponent::GetCurrentAnimationTime() const
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

void AnimatorComponent::SetCurrentAnimationTime(const float animationTime)
{
    //現在のアニメーションが設定されている場合
    if (!currentAnimation_.empty())
    {
        //現在のアニメーションの時間を設定
        SetAnimationTime(currentAnimation_, animationTime);
    }
}

const float AnimatorComponent::GetNextAnimationTime() const
{
    //ブレンド中でかつ次のアニメーションが設定されている場合
    if (isBlending_ && !nextAnimation_.empty())
    {
        //次のアニメーションの時間を返す
        return GetAnimationTime(nextAnimation_);
    }
    return 0.0f;
}

void AnimatorComponent::SetNextAnimationTime(const float animationTime)
{
    //ブレンド中でかつ次のアニメーションが設定されている場合
    if (isBlending_ && !nextAnimation_.empty())
    {
        //次のアニメーションの時間を設定
        SetAnimationTime(nextAnimation_, animationTime);
    }
}

const float AnimatorComponent::GetAnimationSpeed(const std::string& animationName) const
{
    //指定された名前のアニメーションを取得
    Animation* animation = GetAnimation(animationName);
    //アニメーションが存在する場合、そのアニメーションの速度を返す
    //アニメーションが存在しない場合は0.0fを返す
    return animation ? animation->GetAnimationSpeed() : 0.0f;
}

void AnimatorComponent::SetAnimationSpeed(const std::string& animationName, const float animationSpeed)
{
    //指定された名前のアニメーションを取得
    Animation* animation = GetAnimation(animationName);
    //アニメーションが存在する場合、そのアニメーションの時間を設定
    if (animation)
    {
        animation->SetAnimationSpeed(animationSpeed);
    }
}

const float AnimatorComponent::GetCurrentAnimationSpeed() const
{
    //現在のアニメーションが設定されていない場合
    if (currentAnimation_.empty())
    {
        //アニメーションがないので0.0fを返す
        return 0.0f;
    }

    //現在のアニメーションの速度を返す
    return GetAnimationSpeed(currentAnimation_);
}

void AnimatorComponent::SetCurrentAnimationSpeed(const float animationSpeed)
{
    //現在のアニメーションが設定されている場合
    if (!currentAnimation_.empty())
    {
        //現在のアニメーションの速度を設定
        SetAnimationSpeed(currentAnimation_, animationSpeed);
    }
}

const float AnimatorComponent::GetNextAnimationSpeed() const
{
    //ブレンド中でかつ次のアニメーションが設定されている場合
    if (isBlending_ && !nextAnimation_.empty())
    {
        //次のアニメーションの速度を返す
        return GetAnimationSpeed(nextAnimation_);
    }
    return 0.0f;
}

void AnimatorComponent::SetNextAnimationSpeed(const float animationSpeed)
{
    //ブレンド中でかつ次のアニメーションが設定されている場合
    if (isBlending_ && !nextAnimation_.empty())
    {
        //次のアニメーションの速度を設定
        SetAnimationSpeed(nextAnimation_, animationSpeed);
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