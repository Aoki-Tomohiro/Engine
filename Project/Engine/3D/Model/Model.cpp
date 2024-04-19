#include "Model.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Math/MathFunction.h"

void Model::Create(const ModelData& modelData, const Animation& animationData, DrawPass drawPass)
{
	//モデルデータを設定
	modelData_ = modelData;

	//アニメーションデータを設定
	animationData_ = animationData;

	//スケルトンを作成
	skeletonData_ = CreateSkeleton(modelData_.rootNode);

	//SkinClusterを作成
	skinCluster_ = CreateSkinCluster(skeletonData_, modelData_);

	//描画パスを設定
	drawPass_ = drawPass;

	//テクスチャを読み込む
	if (modelData_.material.textureFilePath != "")
	{
		TextureManager::Load(modelData_.material.textureFilePath);
		texture_ = TextureManager::GetInstance()->FindTexture(modelData_.material.textureFilePath);
	}
	else
	{
		texture_ = TextureManager::GetInstance()->FindTexture("white.png");
	}

	//頂点バッファの作成
	CreateVertexBuffer();

	//Indexバッファの作成
	CreateIndexBuffer();

	//マテリアル用のリソースの作成
	CreateMaterialConstBuffer();
}

void Model::Update(WorldTransform& worldTransform)
{
	//全てのJointを更新。親が若いので通常ループで処理可能になっている
	for (Joint& joint : skeletonData_.joints)
	{
		joint.localMatrix = Mathf::MakeAffineMatrix(joint.scale, joint.rotate, joint.translate);
		//親がいれば親の行列を掛ける
		if (joint.parent)
		{
			joint.skeletonSpaceMatrix = joint.localMatrix * skeletonData_.joints[*joint.parent].skeletonSpaceMatrix;
		}
		//親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
		else
		{
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
	}

	//SkeletonSpaceの情報を基に、SkinClusterのMatrixPaletteを更新する
	for (size_t jointIndex = 0; jointIndex < skeletonData_.joints.size(); ++jointIndex)
	{
		assert(jointIndex < skinCluster_.inverseBindPoseMatrices.size());
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix = skinCluster_.inverseBindPoseMatrices[jointIndex] * skeletonData_.joints[jointIndex].skeletonSpaceMatrix;
		skinCluster_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Mathf::Transpose(Mathf::Inverse(skinCluster_.mappedPalette[jointIndex].skeletonSpaceMatrix));
	}
}

void Model::ApplyAnimation()
{
	animationTime_ += 1.0f / 60.0f;//時刻を進める。1/60で固定してあるが、計測した時間を使って可変フレーム対応する方が望ましい
	animationTime_ = std::fmod(animationTime_, animationData_.duration);//最後までいったら最初からリピート再生。リピートしなくても別にいい
	
	for (Joint& joint : skeletonData_.joints)
	{
		//対象のJointのAnimationがあれば、他の適用を行う。下記のif文はc++17から可能になった初期化付きif文
		if (auto it = animationData_.nodeAnimations.find(joint.name); it != animationData_.nodeAnimations.end())
		{
			const NodeAnimation& rootNodeAnimation = (*it).second;
			joint.translate = CalculateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			joint.rotate = CalculateValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			joint.scale = CalculateValue(rootNodeAnimation.scale.keyframes, animationTime_);
		}
	}
}

void Model::UpdateMaterailConstBuffer()
{
	//行列の作成
	Matrix4x4 uvTransformMatrix = Mathf::MakeScaleMatrix({ uvScale_.x,uvScale_.y,1.0f });
	uvTransformMatrix = uvTransformMatrix * Mathf::MakeRotateZMatrix(uvRotation_);
	uvTransformMatrix = uvTransformMatrix * Mathf::MakeTranslateMatrix({ uvTranslation_.x,uvTranslation_.y,0.0f });

	//マテリアルデータの更新
	ConstBuffDataMaterial* materialData = static_cast<ConstBuffDataMaterial*>(materialConstBuffer_->Map());
	materialData->color = color_;
	materialData->uvTransform = uvTransformMatrix;
	materialData->enableLighting = enableLighting_;
	materialData->diffuseReflectionType = diffuseReflectionType_;
	materialData->specularReflectionType = specularReflectionType_;
	materialData->shininess = shininess_;
	materialData->specularColor = specularColor_;
	materialConstBuffer_->Unmap();
}

void Model::Draw(WorldTransform& worldTransform, const Camera& camera)
{
	//マテリアルの更新
	UpdateMaterailConstBuffer();

	//RootのMatrixを適用
	worldTransform.matWorld_ = modelData_.rootNode.localMatrix * worldTransform.matWorld_;
	worldTransform.TransferMatrix();

	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();
	//SortObjectの追加
	renderer_->AddObject(vertexBufferView_, skinCluster_.influenceBufferView, indexBufferView_, materialConstBuffer_->GetGpuVirtualAddress(),
		worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
		texture_->GetSRVHandle(), UINT(modelData_.vertices.size()), drawPass_);
}

void Model::CreateVertexBuffer()
{
	//頂点バッファを作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);

	//頂点バッファにデータを書き込む
	VertexDataPosUVNormal* vertexData = static_cast<VertexDataPosUVNormal*>(vertexBuffer_->Map());
	std::memcpy(vertexData, modelData_.vertices.data(), sizeof(VertexDataPosUVNormal) * modelData_.vertices.size());
	vertexBuffer_->Unmap();
}

void Model::CreateIndexBuffer()
{
	indexBuffer_ = std::make_unique<UploadBuffer>();
	indexBuffer_->Create(sizeof(uint32_t) * modelData_.indices.size());

	indexBufferView_.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
	std::memcpy(indexData, modelData_.indices.data(), sizeof(uint32_t) * modelData_.indices.size());
	indexBuffer_->Unmap();
}

void Model::CreateMaterialConstBuffer()
{
	//マテリアル用のリソースの作成
	materialConstBuffer_ = std::make_unique<UploadBuffer>();
	materialConstBuffer_->Create(sizeof(ConstBuffDataMaterial));

	//マテリアル用のリソースに書き込む
	UpdateMaterailConstBuffer();
}

void Model::SetTexture(const std::string& textureName)
{
	//テクスチャを設定
	texture_ = TextureManager::GetInstance()->FindTexture(textureName);
	//テクスチャがなかったら止める
	assert(texture_);
}

Vector3 Model::CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time)
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

Quaternion Model::CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time)
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

Model::Skelton Model::CreateSkeleton(const Node& rootNode)
{
	Model::Skelton skeleton;
	skeleton.root = CreateJoint(rootNode, {}, skeleton.joints);

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Model::Joint& joint : skeleton.joints)
	{
		skeleton.jointMap.emplace(joint.name, joint.index);
	}

	return skeleton;
}

int32_t Model::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
{
	Model::Joint joint;
	joint.name = node.name;
	joint.localMatrix = node.localMatrix;
	joint.skeletonSpaceMatrix = Mathf::MakeIdentity4x4();
	joint.translate = node.translate;
	joint.rotate = node.rotate;
	joint.scale = node.scale;
	joint.index = int32_t(joints.size());//現在登録されている数をIndexに
	joint.parent = parent;
	joints.push_back(joint);//SkeletonのJoint列に追加
	for (const Model::Node& child : node.children)
	{
		//子Jointを作成し、そのIndexを登録
		int32_t childIndex = CreateJoint(child, joint.index, joints);
		joints[joint.index].children.push_back(childIndex);
	}
	//自身のIndexを返す
	return joint.index;
}

Model::SkinCluster Model::CreateSkinCluster(const Skelton& skeleton, const ModelData& modelData)
{
	//palette用のResourceを確保
	SkinCluster skinCluster;
	skinCluster.paletteResource = std::make_unique<StructuredBuffer>();
	skinCluster.paletteResource->Create(uint32_t(skeleton.joints.size()), sizeof(WellForGPU));
	WellForGPU* mappedPalette = static_cast<WellForGPU*>(skinCluster.paletteResource->Map());
	skinCluster.mappedPalette = { mappedPalette,skeleton.joints.size() };//spanを使ってアクセスするようにする
	skinCluster.paletteResource->Unmap();

	//influence用のResourceを確保。頂点ごとにinfluenced情報を追加できるようにする
	skinCluster.influenceResource = std::make_unique<UploadBuffer>();
	skinCluster.influenceResource->Create(sizeof(VertexInfluence) * modelData.vertices.size());
	VertexInfluence* mappedInfluence = static_cast<VertexInfluence*>(skinCluster.influenceResource->Map());
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData.vertices.size());//0埋め。weightを0にしておく。
	skinCluster.influenceResource->Unmap();

	//Influence用のVBVを作成
	skinCluster.influenceBufferView.BufferLocation = skinCluster.influenceResource->GetGpuVirtualAddress();
	skinCluster.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * modelData.vertices.size());
	skinCluster.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
	skinCluster.inverseBindPoseMatrices.resize(skeleton.joints.size());
	for (Matrix4x4 inverseBindPoseMatrix : skinCluster.inverseBindPoseMatrices)
	{
		inverseBindPoseMatrix = Mathf::MakeIdentity4x4();
	}
	//std::generate(skinCluster.inverseBindPoseMatrices.begin(), skinCluster.inverseBindPoseMatrices.end(), Mathf::MakeIdentity4x4());

	//ModelDataを解析してInfluenceを埋める
	for (const auto& jointWeight : modelData.skinClusterData)//ModelのSkinClusterの情報を解析
	{
		auto it = skeleton.jointMap.find(jointWeight.first);//JointWeight,firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		if (it == skeleton.jointMap.end())//そんな名前のJointは存在しない。なので次に回す
		{
			continue;
		}

		//(*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
		skinCluster.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights)
		{
			auto& currentInfluence = skinCluster.mappedInfluence[vertexWeight.vertexIndex];//該当のvertexIndexのinfluence情報を参照しておく
			for (uint32_t index = 0; index < kNumMaxInfluence; ++index)//空いているところに入れる
			{
				if (currentInfluence.weights[index] == 0.0f)//Weight == 0が空いている状態なので、その場所にweightとjointのindexを代入
				{
					currentInfluence.weights[index] = vertexWeight.weight;
					currentInfluence.jointIndices[index] = (*it).second;
					break;
				}
			}
		}
	}

	return skinCluster;
}
