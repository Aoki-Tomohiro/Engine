#include "Mesh.h"
#include "Engine/Math/MathFunction.h"
#include <cassert>

void Mesh::Initialize(const MeshData& meshData, const Animation::Skeleton& skeleton, const std::map<std::string, JointWeightData>& skinClusterData)
{
	//頂点データの初期化
	meshData_ = meshData;

	//頂点バッファの作成
	CreateVertexBuffer();

	//インデックスバッファの作成
	CreateIndexBuffer();

	//SkinClusterの作成
	if (!skinClusterData.empty())
	{
		hasSkinCluster_ = true;
		CreateSkinCluster(skeleton, skinClusterData);
	}
}

void Mesh::Update(const std::vector<Animation::Joint>& joints)
{
	if (hasSkinCluster_)
	{
		for (size_t jointIndex = 0; jointIndex < joints.size(); ++jointIndex)
		{
			assert(jointIndex < skinClusterData_.inverseBindPoseMatrices.size());
			skinClusterData_.mappedPalette[jointIndex].skeletonSpaceMatrix = skinClusterData_.inverseBindPoseMatrices[jointIndex] * joints[jointIndex].skeletonSpaceMatrix;
			skinClusterData_.mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Mathf::Transpose(Mathf::Inverse(skinClusterData_.mappedPalette[jointIndex].skeletonSpaceMatrix));
		}
	}
}

void Mesh::CreateSkinCluster(const Animation::Skeleton& skeleton, const std::map<std::string, JointWeightData>& skinClusterData)
{
	//palette用のResourceを確保
	skinClusterData_.paletteResource = std::make_unique<StructuredBuffer>();
	skinClusterData_.paletteResource->Create(uint32_t(skeleton.joints.size()), sizeof(WellForGPU));
	WellForGPU* mappedPalette = static_cast<WellForGPU*>(skinClusterData_.paletteResource->Map());
	skinClusterData_.mappedPalette = { mappedPalette,skeleton.joints.size() };//spanを使ってアクセスするようにする

	//influence用のResourceを確保。頂点ごとにinfluenced情報を追加できるようにする
	skinClusterData_.influenceResource = std::make_unique<UploadBuffer>();
	skinClusterData_.influenceResource->Create(sizeof(VertexInfluence) * meshData_.vertices.size());
	VertexInfluence* mappedInfluence = static_cast<VertexInfluence*>(skinClusterData_.influenceResource->Map());
	std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * meshData_.vertices.size());//0埋め。weightを0にしておく。
	skinClusterData_.mappedInfluence = { mappedInfluence, meshData_.vertices.size() };

	//Influence用のVBVを作成
	skinClusterData_.influenceBufferView.BufferLocation = skinClusterData_.influenceResource->GetGpuVirtualAddress();
	skinClusterData_.influenceBufferView.SizeInBytes = UINT(sizeof(VertexInfluence) * meshData_.vertices.size());
	skinClusterData_.influenceBufferView.StrideInBytes = sizeof(VertexInfluence);

	//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
	skinClusterData_.inverseBindPoseMatrices.resize(skeleton.joints.size());
	for (Matrix4x4& inverseBindPoseMatrix : skinClusterData_.inverseBindPoseMatrices)
	{
		inverseBindPoseMatrix = Mathf::MakeIdentity4x4();
	}

	//ModelDataを解析してInfluenceを埋める
	for (const auto& jointWeight : skinClusterData)//ModelのSkinClusterの情報を解析
	{
		auto it = skeleton.jointMap.find(jointWeight.first);//JointWeight,firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
		if (it == skeleton.jointMap.end())//そんな名前のJointは存在しない。なので次に回す
		{
			continue;
		}

		//(*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
		skinClusterData_.inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
		for (const auto& vertexWeight : jointWeight.second.vertexWeights)
		{
			auto& currentInfluence = skinClusterData_.mappedInfluence[vertexWeight.vertexIndex];//該当のvertexIndexのinfluence情報を参照しておく
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
}

void Mesh::CreateVertexBuffer()
{
	//頂点バッファを作成
	vertexBuffer_ = std::make_unique<UploadBuffer>();
	vertexBuffer_->Create(sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());

	//頂点バッファビューを作成
	vertexBufferView_.BufferLocation = vertexBuffer_->GetGpuVirtualAddress();
	vertexBufferView_.SizeInBytes = UINT(sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
	vertexBufferView_.StrideInBytes = sizeof(VertexDataPosUVNormal);

	//頂点バッファにデータを書き込む
	VertexDataPosUVNormal* vertexData = static_cast<VertexDataPosUVNormal*>(vertexBuffer_->Map());
	std::memcpy(vertexData, meshData_.vertices.data(), sizeof(VertexDataPosUVNormal) * meshData_.vertices.size());
	vertexBuffer_->Unmap();
}

void Mesh::CreateIndexBuffer()
{
	//インデックスバッファの作成
	indexBuffer_ = std::make_unique<UploadBuffer>();
	indexBuffer_->Create(sizeof(uint32_t) * meshData_.indices.size());

	//インデックスバッファビューの作成
	indexBufferView_.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
	indexBufferView_.SizeInBytes = UINT(sizeof(uint32_t) * meshData_.indices.size());
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;

	//インデックスバッファのデータを書き込む
	uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
	std::memcpy(indexData, meshData_.indices.data(), sizeof(uint32_t) * meshData_.indices.size());
	indexBuffer_->Unmap();
}