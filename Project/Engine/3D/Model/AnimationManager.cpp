/**
 * @file AnimationManager.cpp
 * @brief アニメーションデータの読み込みや管理をするファイル
 * @author 青木智滉
 * @date
 */

#include "AnimationManager.h"

AnimationManager* AnimationManager::instance_ = nullptr;
const std::string AnimationManager::kBaseDirectory = "Application/Resources/Models";

AnimationManager* AnimationManager::GetInstance()
{
	if (instance_ == nullptr)
	{
		instance_ = new AnimationManager();
	}
	return instance_;
}

void AnimationManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

Animation* AnimationManager::Create(const std::string& fileName)
{
	Animation* animation = AnimationManager::GetInstance()->CreateInternal(fileName);
	return animation;
}

Animation* AnimationManager::CreateInternal(const std::string& fileName)
{
	//同じアニメーションがないかチェック
	auto it = animationDatas_.find(fileName);
	if (it != animationDatas_.end())
	{
		Animation* animation = new Animation();
		animation->Initialize(it->second);
		return animation;
	}

	//アニメーションデータの読み込み
	std::vector<Animation::AnimationData> animationDatas = LoadAnimationFile(kBaseDirectory, fileName);
	//アニメーションデータを保存
	animationDatas_[fileName] = animationDatas;

	//アニメーションの生成
	Animation* animation = new Animation();
	animation->Initialize(animationDatas);

	return animation;
}

std::vector<Animation::AnimationData> AnimationManager::LoadAnimationFile(const std::string& directoryPath, const std::string& filename)
{
	std::vector<Animation::AnimationData> animation{};//今回作るアニメーション
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	if (scene->mAnimations == 0) return animation;//アニメーションがない
	for (uint32_t animationIndex = 0; animationIndex < scene->mNumAnimations; animationIndex++)
	{
		Animation::AnimationData currentAnimationData{};
		aiAnimation* animationAssimp = scene->mAnimations[animationIndex];//最初のアニメーションだけ採用。もちろん複数対応することに越したことはない
		currentAnimationData.name = animationAssimp->mName.C_Str();
		currentAnimationData.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond);//時間の単位を秒に変換
		//assimpでは個々のNodeのAnimationをchannelと読んでいるのでchannelを回してNodeAnimationの情報を取ってくる
		for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumChannels; ++channelIndex)
		{
			aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
			Animation::NodeAnimation& nodeAnimation = currentAnimationData.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
			//Translate
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; ++keyIndex)
			{
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
				Animation::KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);//ここも秒に変換
				keyframe.value = { -keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };//右手->左手
				nodeAnimation.translate.keyframes.push_back(keyframe);
			}
			//Rotate
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; ++keyIndex)
			{
				aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
				Animation::KeyframeQuaternion keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x,-keyAssimp.mValue.y,-keyAssimp.mValue.z,keyAssimp.mValue.w };
				nodeAnimation.rotate.keyframes.push_back(keyframe);
			}
			//Scale
			for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; ++keyIndex)
			{
				aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
				Animation::KeyframeVector3 keyframe;
				keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
				keyframe.value = { keyAssimp.mValue.x,keyAssimp.mValue.y,keyAssimp.mValue.z };
				nodeAnimation.scale.keyframes.push_back(keyframe);
			}
		}
		animation.push_back(currentAnimationData);
	}
	//解析完了
	return animation;
}