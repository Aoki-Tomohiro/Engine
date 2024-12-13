/**
 * @file Animation.cpp
 * @brief アニメーションを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "Animation.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include <cassert>
#include <algorithm>

void Animation::Initialize(const std::vector<AnimationData>& animationData)
{
    //アニメーションデータの初期化
    animationDatas_ = animationData;

    //最初のアニメーションを設定
    animationName_ = animationDatas_.empty() ? "" : animationDatas_[0].name;
}

void Animation::UpdateAnimationTime()
{
    //アニメーションの停止フラグが立っている場合は何もしない
    if (stop_ || pause_) return;

    //現在のアニメーションデータを取得
    const AnimationData* animationData = GetAnimationData();
    //現在のアニメーションデータがない場合は何もしない
    if (!animationData) return;

    //アニメーション時間を進める
    animationTime_ += animationSpeed_ * GameTimer::GetDeltaTime();

    //アニメーションの終了時間を超えた場合
    if (animationTime_ >= animationData->duration)
    {
        //ループフラグが立っている場合アニメーションをループさせる
        if (loop_)
        {
            animationTime_ = std::fmod(animationTime_, animationData->duration);
        }
        //ループフラグが立っていない場合アニメーションを停止し終了フラグをセット
        else
        {
            animationTime_ = animationData->duration;
            isAnimationFinished_ = true;
        }
    }
    //アニメーションが終了していない場合終了フラグをリセット
    else
    {
        isAnimationFinished_ = false;
    }
}

void Animation::ApplyAnimation(Model* model, WorldTransform& worldTransform, const Vector3& inPlaceAxis)
{
    //アニメーションデータを取得
    const AnimationData* animationData = GetAnimationData();
    //現在のアニメーションデータがない場合は何もしない
    if (!animationData) return;

    //アニメーション時間を更新する
    UpdateAnimationTime();

    //ノードレベルのアニメーションを適用する
    ApplyNodeAnimation(model, worldTransform, *animationData);

    //スケルトンレベルのアニメーションを適用する
    ApplySkeletonAnimation(model, *animationData, inPlaceAxis);
}

void Animation::ApplyBlendAnimation(Model* model, WorldTransform& worldTransform, Animation* blendAnimation, const float blendFactor, const Vector3& inPlaceAxis)
{
    //現在のアニメーションデータを取得
    const AnimationData* currentAnimationData = GetAnimationData();
    //現在のアニメーションデータがない場合は何もしない
    if (!currentAnimationData) return;

    //アニメーション時間を更新する
    UpdateAnimationTime();
    //ブレンドするアニメーションの時間を更新する
    blendAnimation->UpdateAnimationTime();

    //ノードのブレンドアニメーションを適用する
    ApplyBlendedNodeAnimation(model, worldTransform, *currentAnimationData, blendAnimation, blendFactor);

    //スケルトンのブレンドアニメーションを適用する
    ApplyBlendedSkeletonAnimation(model, *currentAnimationData, blendAnimation, blendFactor, inPlaceAxis);
}

void Animation::PlayAnimation(const std::string& animationName, float speed, bool loop)
{
    //アニメーション名を設定
    animationName_ = animationName; 
    //ループフラグを設定
    loop_ = loop; 
    //停止フラグを解除
    stop_ = false;
    //一時停止フラグを解除
    pause_ = false;
    //アニメーションの終了フラグを設定
    isAnimationFinished_ = false;
    //アニメーション時間をリセット
    animationTime_ = 0.0f; 
    //アニメーション速度を設定
    animationSpeed_ = speed;
}

void Animation::PlayAnimation(const float speed, const bool loop)
{
    //ループフラグを設定
    loop_ = loop;
    //停止フラグを解除
    stop_ = false;
    //一時停止フラグを解除
    pause_ = false;
    //アニメーションの終了フラグを設定
    isAnimationFinished_ = false;
    //アニメーション時間をリセット
    animationTime_ = 0.0f;
    //アニメーション速度を設定
    animationSpeed_ = speed;
}

void Animation::StopAnimation()
{
    //停止フラグを設定
    stop_ = true;
    //一時停止フラグを解除
    pause_ = false;
    //アニメーションの終了フラグを設定
    isAnimationFinished_ = false;
    //アニメーション時間をリセット
    animationTime_ = 0.0f;
}

void Animation::PauseAnimation()
{
    //一時停止させる
    pause_ = true;
}

void Animation::ResumeAnimation()
{
    //一時停止を解除
    pause_ = false;
}

const float Animation::GetDuration() const
{
    const AnimationData* currentData = GetAnimationData();
    return currentData ? currentData->duration : 0.0f;
}

const Animation::AnimationData* Animation::GetAnimationData() const
{
    auto it = std::find_if(animationDatas_.begin(), animationDatas_.end(),
        [this](const AnimationData& data) { return data.name == animationName_; });
    return it != animationDatas_.end() ? &(*it) : nullptr;
}

Vector3 Animation::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
{
    assert(!keyframes.empty());//キーがないものは返す値が分からないのでダメ
    //キーが2つか、時刻がキーフレーム前なら最初の値とする
    if (keyframes.size() == 1 || time <= keyframes[0].time)
    {
        return keyframes[0].value;
    }

    for (size_t index = 0; index < keyframes.size() - 1; ++index)
    {
        size_t nextIndex = index + 1;
        //indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
        if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
        {
            //範囲内を補間する
            float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
            return Mathf::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
        }
    }
    return (*keyframes.rbegin()).value;
}

Quaternion Animation::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
{
    assert(!keyframes.empty());//キーがないものは返す値が分からないのでダメ
    //キーが2つか、時刻がキーフレーム前なら最初の値とする
    if (keyframes.size() == 1 || time <= keyframes[0].time)
    {
        return keyframes[0].value;
    }

    for (size_t index = 0; index < keyframes.size() - 1; ++index)
    {
        size_t nextIndex = index + 1;
        //indexとnextIndexの2つのkeyframeを取得して範囲内に時刻があるかを判定
        if (keyframes[index].time <= time && time <= keyframes[nextIndex].time)
        {
            //範囲内を補間する
            float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
            return Mathf::Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
        }
    }
    return (*keyframes.rbegin()).value;
}

void Animation::ApplyNodeAnimation(Model* model, WorldTransform& worldTransform, const AnimationData& animationData)
{
    //ノードアニメーションを適用する
    if (auto it = animationData.nodeAnimations.find(model->GetRootNode().name); it != animationData.nodeAnimations.end())
    {
        //ノードアニメーションを取得
        const NodeAnimation& rootNodeAnimation = it->second;
        //現在のアニメーション時間に基づいて、変換、回転、スケーリングを計算
        Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
        Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
        Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
        //計算した変換、回転、スケーリングからローカル行列を作成
        Matrix4x4 localMatrix = Mathf::MakeAffineMatrix(scale, rotate, translate);
        //ワールド行列にローカル行列を適用
        worldTransform.matWorld_ = localMatrix * worldTransform.matWorld_;
    }
}

void Animation::ApplySkeletonAnimation(Model* model, const AnimationData& animationData, const Vector3& inPlaceAxis)
{
    //スケルトンを取得
    Model::Skeleton& skeleton = model->GetSkeleton();

    //スケルトン内の全ジョイントに対してアニメーションを適用
    for (Model::Joint& joint : skeleton.joints)
    {
        //ジョイントの名前でアニメーションデータを検索
        if (auto it = animationData.nodeAnimations.find(joint.name); it != animationData.nodeAnimations.end())
        {
            //ノードアニメーションを取得
            const NodeAnimation& rootNodeAnimation = it->second;
            //現在のアニメーション時間に基づいて、変換、回転、スケーリングを計算
            joint.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
            joint.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
            joint.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);

            //階層のトップジョイントの場合は位置成分をリセット
            if (joint.name.find("Root") != std::string::npos || joint.name.find("Hips") != std::string::npos)
            {
                joint.translate = {
                    inPlaceAxis.x != 0.0f ? joint.translate.x : 0.0f,
                    inPlaceAxis.y != 0.0f ? joint.translate.y : 0.0f,
                    inPlaceAxis.z != 0.0f ? joint.translate.z : 0.0f
                };
            }
        }
    }
}

void Animation::ApplyBlendedNodeAnimation(Model* model, WorldTransform& worldTransform, const AnimationData& currentAnimationData, const Animation* blendAnimation, const float blendFactor)
{
    //モデルのルートノードの名前で現在のアニメーションデータを検索
    if (auto it = currentAnimationData.nodeAnimations.find(model->GetRootNode().name); it != currentAnimationData.nodeAnimations.end())
    {
        //ノードアニメーションを取得
        const NodeAnimation& rootNodeAnimation = it->second;
        //現在のアニメーション時間に基づいて、変換、回転、スケーリングを計算
        Vector3 currentTranslate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
        Quaternion currentRotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
        Vector3 currentScale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);

        //ブレンドアニメーションが存在する場合
        if (blendAnimation)
        {
            //アニメーションデータを取得
            const AnimationData* blendAnimationData = blendAnimation->GetAnimationData();
            //アニメーションデータが存在する場合
            if (blendAnimationData)
            {
                //モデルのルートノードの名前で現在のアニメーションデータを検索
                if (auto blendIt = blendAnimationData->nodeAnimations.find(model->GetRootNode().name); blendIt != blendAnimationData->nodeAnimations.end())
                {
                    //ノードアニメーションを取得
                    const NodeAnimation& blendNodeAnimation = blendIt->second;
                    //ブレンドアニメーションに基づいて、変換、回転、スケーリングを計算
                    Vector3 blendTranslate = CalculateValue(blendNodeAnimation.translate.keyframes, blendAnimation->GetAnimationTime());
                    Quaternion blendRotate = CalculateValue(blendNodeAnimation.rotate.keyframes, blendAnimation->GetAnimationTime());
                    Vector3 blendScale = CalculateValue(blendNodeAnimation.scale.keyframes, blendAnimation->GetAnimationTime());

                    //現在のアニメーションとブレンドアニメーションを補間する
                    currentTranslate = Mathf::Lerp(currentTranslate, blendTranslate, blendFactor);
                    currentRotate = Mathf::Slerp(currentRotate, blendRotate, blendFactor);
                    currentScale = Mathf::Lerp(currentScale, blendScale, blendFactor);
                }
            }
        }

        //計算した変換、回転、スケーリングからローカル行列を作成
        Matrix4x4 localMatrix = Mathf::MakeAffineMatrix(currentScale, currentRotate, currentTranslate);

        //ワールド行列にローカル行列を適用
        worldTransform.matWorld_ = localMatrix * worldTransform.matWorld_;
    }
}

void Animation::ApplyBlendedSkeletonAnimation(Model* model, const AnimationData& currentAnimationData, const Animation* blendAnimation, const float blendFactor, const Vector3& inPlaceAxis)
{
    //スケルトンを取得
    Model::Skeleton& skeleton = model->GetSkeleton();

    //スケルトン内の全ジョイントに対してアニメーションを適用
    for (Model::Joint& joint : skeleton.joints)
    {
        //ジョイントの名前で現在のアニメーションデータを検索
        if (auto it = currentAnimationData.nodeAnimations.find(joint.name); it != currentAnimationData.nodeAnimations.end())
        {
            //ノードアニメーションを取得
            const NodeAnimation& rootNodeAnimation = it->second;
            //現在のアニメーション時間に基づいて、変換、回転、スケーリングを計算
            Vector3 currentTranslate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
            Quaternion currentRotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
            Vector3 currentScale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);

            //ブレンドアニメーションが存在する場合
            if (blendAnimation)
            {
                //アニメーションデータを取得
                const AnimationData* blendAnimationData = blendAnimation->GetAnimationData();
                //アニメーションデータが存在する場合
                if (blendAnimationData)
                {
                    //モデルのルートノードの名前で現在のアニメーションデータを検索
                    if (auto blendIt = blendAnimationData->nodeAnimations.find(joint.name); blendIt != blendAnimationData->nodeAnimations.end())
                    {
                        //ノードアニメーションを取得
                        const NodeAnimation& blendNodeAnimation = blendIt->second;
                        //ブレンドアニメーションに基づいて、変換、回転、スケーリングを計算
                        Vector3 blendTranslate = CalculateValue(blendNodeAnimation.translate.keyframes, blendAnimation->GetAnimationTime());
                        Quaternion blendRotate = CalculateValue(blendNodeAnimation.rotate.keyframes, blendAnimation->GetAnimationTime());
                        Vector3 blendScale = CalculateValue(blendNodeAnimation.scale.keyframes, blendAnimation->GetAnimationTime());

                        //現在のアニメーションとブレンドアニメーションを補間する
                        currentTranslate = Mathf::Lerp(currentTranslate, blendTranslate, blendFactor);
                        currentRotate = Mathf::Slerp(currentRotate, blendRotate, blendFactor);
                        currentScale = Mathf::Lerp(currentScale, blendScale, blendFactor);
                    }
                }
            }

            //ジョイントの変換を更新する
            joint.translate = currentTranslate;
            joint.rotate = currentRotate;
            joint.scale = currentScale;

            //階層のトップジョイントの場合は位置成分をリセット
            if (joint.name.find("Root") != std::string::npos || joint.name.find("Hips") != std::string::npos)
            {
                joint.translate = {
                    inPlaceAxis.x != 0.0f ? joint.translate.x : 0.0f,
                    inPlaceAxis.y != 0.0f ? joint.translate.y : 0.0f,
                    inPlaceAxis.z != 0.0f ? joint.translate.z : 0.0f
                };
            }
        }
    }
}