#include "Animation.h"
#include "Engine/Math/MathFunction.h"
#include <cassert>

void Animation::Initialize(const std::vector<AnimationData>& animationData, const Node& rootNode)
{
	//アニメーションデータの初期化
	animationData_ = animationData;

	//スケルトンを作成
	skeletonData_ = CreateSkeleton(rootNode);
}

void Animation::Update()
{
	//全てのJointの更新。親が若いので通常ループで処理可能になっている
	for (Joint& joint : skeletonData_.joints)
	{
		joint.localMatrix = Mathf::MakeAffineMatrix(joint.scale, joint.rotate, joint.translate);
		if (joint.parent)//親がいれば親の行列を掛ける
		{
			joint.skeletonSpaceMatrix = joint.localMatrix * skeletonData_.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else//親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
		{
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}
}

void Animation::ApplyAnimation(WorldTransform& worldTransform, const std::string& rootNodeName, const std::string& animationName)
{
	for (AnimationData& animationData : animationData_)
	{
		if (animationData.name != animationName)
		{
			continue;
		}

		//時刻を進める。1/60で固定してあるが、計測した時間を使って可変フレーム対応する方が望ましい
		animationTime_ += 1.0f / 60.0f;
		animationTime_ = std::fmod(animationTime_, animationData.duration);

		//RigidAnimation
		if (auto it = animationData.nodeAnimations.find(rootNodeName); it != animationData.nodeAnimations.end())
		{
			NodeAnimation& rootNodeAnimation = (*it).second;
			Vector3 translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			Quaternion rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			Vector3 scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
			Matrix4x4 localMatrix = Mathf::MakeAffineMatrix(scale, rotate, translate);
			worldTransform.matWorld_ = localMatrix * worldTransform.matWorld_;
		}

		//SkeletonAnimation
		for (Joint& joint : skeletonData_.joints)
		{
			//対象のJointのAnimationがあれば、値の適用を行う。下記のif文はc++17から可能になった初期化付きif文。
			if (auto it = animationData.nodeAnimations.find(joint.name); it != animationData.nodeAnimations.end())
			{
				const NodeAnimation& rootNodeAnimation = (*it).second;
				joint.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
				joint.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
				joint.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
			}
		}
	}
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

Animation::Skeleton Animation::CreateSkeleton(const Node& rootNode)
{
	Skeleton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton.joints)
	{
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

int32_t Animation::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = Mathf::MakeIdentity4x4();
	joint.scale = node.scale;
	joint.rotate = node.rotate;
	joint.translate = node.translate;
	joint.index = int32_t(joints.size());//現在登録されている数をIndexに
	joint.parent = parent;
	joints.push_back(joint);//SkeletonのJoint列に追加
	for (const Node& child : node.children)
	{
		//子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	//自身のIndexを返す
	return joint.index;
}