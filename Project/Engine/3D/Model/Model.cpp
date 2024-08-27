#include "Model.h"
#include "Engine/Math/MathFunction.h"
#include <cassert>

void Model::Initialize(const ModelData& modelData, const DrawPass drawPass)
{
	//モデルデータの初期化
	modelData_ = modelData;

	//描画パスを設定
	drawPass_ = drawPass;

	//メッシュの作成
	CreateMeshes();

	//マテリアルの作成
	CreateMaterials();

	//スケルトンの作成
	CreateSkeleton();

	//ジョイントのワールドトランスフォームを作成
	CreateJointWorldTransforms();

	//スキンクラスターの作成
	CreateSkinClusters();

	//ボーンの作成
	CreateBone();
}

void Model::Update(WorldTransform& worldTransform)
{
	//スケルトンの更新
	UpdateSkeleton(worldTransform);

	//スキンクラスターの更新
	UpdateSkinClusters();

	//マテリアルの更新
	UpdateMaterials();

	//ルートのローカル行列を適用
	ApplyRootTransform(worldTransform);
}

void Model::Draw(const WorldTransform& worldTransform, const Camera& camera)
{
	//レンダラーのインスタンスを取得
	Renderer* renderer_ = Renderer::GetInstance();

	//ソートオブジェクトの追加
	for (uint32_t i = 0; i < meshes_.size(); ++i)
	{
		//マテリアルのインデックスを取得
		uint32_t materialIndex = meshes_[i]->GetMaterialIndex();

		//オブジェクトの追加
		renderer_->AddObject(meshes_[i]->GetVertexBufferView(), meshes_[i]->GetIndexBufferView(), materials_[materialIndex]->GetConstantBuffer()->GetGpuVirtualAddress(),
			worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(),
			materials_[materialIndex]->GetTexture()->GetSRVHandle(), materials_[materialIndex]->GetMaskTexture()->GetSRVHandle(), UINT(meshes_[i]->GetIndicesSize()), drawPass_);

		//スキンクラスターを持っている場合
		if (!modelData_.skinClusterData[i].empty())
		{
			//スキニングオブジェクトの追加
			renderer_->AddSkinningObject(skinClusters_[i].paletteResource->GetSRVHandle(), meshes_[i]->GetInputVerticesBuffer()->GetSRVHandle(),
				skinClusters_[i].influenceResource->GetSRVHandle(), meshes_[i]->GetSkinningInformationBuffer()->GetGpuVirtualAddress(),
				meshes_[i]->GetOutputVerticesBuffer(), UINT(meshes_[i]->GetVerticesSize()));
		}

		//影を描画する場合
		if (castShadows_)
		{
			//影の追加
			renderer_->AddShadowObject(meshes_[i]->GetVertexBufferView(), meshes_[i]->GetIndexBufferView(),
				worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), UINT(meshes_[i]->GetIndicesSize()));
		}
	}

	//スキンクラスターを持っている場合
	if (!boneVertices_.empty() && isBoneVisible_)
	{
		//頂点データの更新
		UpdateBoneVertexData(skeleton_.root);

		//ボーンの追加
		renderer_->AddBone(boneVertexBufferView_, worldTransform.GetConstantBuffer()->GetGpuVirtualAddress(), camera.GetConstantBuffer()->GetGpuVirtualAddress(), UINT(boneVertices_.size()));
	}
}

void Model::CreateSkeleton()
{
	//ジョイントの作成
	skeleton_.root = CreateJoint(modelData_.rootNode, {}, skeleton_.joints);

	//名前とindexのマッピングを行いアクセスしやすくする
	for (const Joint& joint : skeleton_.joints)
	{
		skeleton_.jointMap.emplace(joint.name, joint.index);
	}
}

int32_t Model::CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints)
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

void Model::CreateJointWorldTransforms()
{
	//JointのWorldTransformを作成
	jointWorldTransforms_.resize(skeleton_.joints.size());
	for (uint32_t i = 0; i < skeleton_.joints.size(); ++i)
	{
		jointWorldTransforms_[i].rotationType_ = RotationType::Quaternion;
	}
}

void Model::CreateSkinClusters()
{
	//全てのメッシュ分のスキンクラスターを生成
	skinClusters_.resize(meshes_.size());
	for (int32_t i = 0; i < meshes_.size(); ++i)
	{
		//スキンクラスターデータがなければ飛ばす
		if (modelData_.skinClusterData[i].empty())
		{
			continue;
		}

		//palette用のResourceを確保
		skinClusters_[i].paletteResource = std::make_unique<StructuredBuffer>();
		skinClusters_[i].paletteResource->Create(uint32_t(skeleton_.joints.size()), sizeof(WellForGPU));
		WellForGPU* mappedPalette = static_cast<WellForGPU*>(skinClusters_[i].paletteResource->Map());
		skinClusters_[i].mappedPalette = { mappedPalette,skeleton_.joints.size() };//spanを使ってアクセスするようにする

		//influence用のResourceを確保。頂点ごとにinfluenced情報を追加できるようにする
		skinClusters_[i].influenceResource = std::make_unique<StructuredBuffer>();
		skinClusters_[i].influenceResource->Create((uint32_t)modelData_.meshData[i].vertices.size(), sizeof(VertexInfluence));
		VertexInfluence* mappedInfluence = static_cast<VertexInfluence*>(skinClusters_[i].influenceResource->Map());
		std::memset(mappedInfluence, 0, sizeof(VertexInfluence) * modelData_.meshData[i].vertices.size());//0埋め。weightを0にしておく。
		skinClusters_[i].mappedInfluence = { mappedInfluence, modelData_.meshData[i].vertices.size() };

		//InverseBindPoseMatrixを格納する場所を作成して、単位行列で埋める
		skinClusters_[i].inverseBindPoseMatrices.resize(skeleton_.joints.size());
		for (Matrix4x4& inverseBindPoseMatrix : skinClusters_[i].inverseBindPoseMatrices)
		{
			inverseBindPoseMatrix = Mathf::MakeIdentity4x4();
		}

		//ModelDataを解析してInfluenceを埋める
		for (const auto& jointWeight : modelData_.skinClusterData[i])//ModelのSkinClusterの情報を解析
		{
			auto it = skeleton_.jointMap.find(jointWeight.first);//JointWeight,firstはjoint名なので、skeletonに対象となるjointが含まれているか判断
			if (it == skeleton_.jointMap.end())//そんな名前のJointは存在しない。なので次に回す
			{
				continue;
			}

			//(*it).secondにはjointのindexが入っているので、該当のindexのinverseBindPoseMatrixを代入
			skinClusters_[i].inverseBindPoseMatrices[(*it).second] = jointWeight.second.inverseBindPoseMatrix;
			for (const auto& vertexWeight : jointWeight.second.vertexWeights)
			{
				auto& currentInfluence = skinClusters_[i].mappedInfluence[vertexWeight.vertexIndex];//該当のvertexIndexのinfluence情報を参照しておく
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
}

void Model::CreateMeshes()
{
	//メッシュの作成
	for (int32_t i = 0; i < modelData_.meshData.size(); ++i)
	{
		Mesh* mesh = new Mesh();
		mesh->Initialize(modelData_.meshData[i], !modelData_.skinClusterData[i].empty());
		meshes_.push_back(std::unique_ptr<Mesh>(mesh));
	}
}

void Model::CreateMaterials()
{
	//マテリアルの作成
	for (uint32_t i = 0; i < modelData_.materialData.size(); ++i)
	{
		Material* material = new Material();
		material->Initialize(modelData_.materialData[i]);
		material->Update();
		materials_.push_back(std::unique_ptr<Material>(material));
	}
}

void Model::CreateBone()
{
	//ボーンの頂点データを作成
	CreateBoneVertices(skeleton_.root);

	//ボーンが空ではない場合は頂点バッファを作成
	if (!boneVertices_.empty())
	{
		CreateBoneVertexBuffer();
	}
}

void Model::CreateBoneVertices(int32_t parentIndex)
{
	const Joint& parentJoint = skeleton_.joints[parentIndex];
	for (int32_t childIndex : parentJoint.children)
	{
		const Joint& childJoint = skeleton_.joints[childIndex];
		boneVertices_.push_back({ parentJoint.skeletonSpaceMatrix.m[3][0],parentJoint.skeletonSpaceMatrix.m[3][1],parentJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		boneVertices_.push_back({ childJoint.skeletonSpaceMatrix.m[3][0],childJoint.skeletonSpaceMatrix.m[3][1],childJoint.skeletonSpaceMatrix.m[3][2],1.0f });
		CreateBoneVertices(childIndex);
	}
}

void Model::UpdateBoneVertexData(int32_t parentIndex)
{
	//頂点データをクリア
	boneVertices_.clear();

	//頂点データの作成
	CreateBoneVertices(skeleton_.root);

	//頂点データの書き込み
	Vector4* vertexData = static_cast<Vector4*>(boneVertexBuffer_->Map());
	std::memcpy(vertexData, boneVertices_.data(), sizeof(Vector4) * boneVertices_.size());
	boneVertexBuffer_->Unmap();
}

void Model::CreateBoneVertexBuffer()
{
	//頂点バッファを作成
	boneVertexBuffer_ = std::make_unique<UploadBuffer>();
	boneVertexBuffer_->Create(sizeof(Vector4) * boneVertices_.size());

	//頂点バッファビューを作成
	boneVertexBufferView_.BufferLocation = boneVertexBuffer_->GetGpuVirtualAddress();
	boneVertexBufferView_.SizeInBytes = UINT(sizeof(Vector4) * boneVertices_.size());
	boneVertexBufferView_.StrideInBytes = sizeof(Vector4);

	//頂点バッファにデータを書き込む
	Vector4* vertexData = static_cast<Vector4*>(boneVertexBuffer_->Map());
	std::memcpy(vertexData, boneVertices_.data(), sizeof(Vector4) * boneVertices_.size());
	boneVertexBuffer_->Unmap();
}

void Model::UpdateSkeleton(const WorldTransform& worldTransform)
{
	//全てのJointの更新。親が若いので通常ループで処理可能になっている
	for (Joint& joint : skeleton_.joints)
	{
		joint.localMatrix = Mathf::MakeAffineMatrix(joint.scale, joint.rotate, joint.translate);
		if (joint.parent)//親がいれば親の行列を掛ける
		{
			joint.skeletonSpaceMatrix = joint.localMatrix * skeleton_.joints[*joint.parent].skeletonSpaceMatrix;
		}
		else//親がいないのでlocalMatrixとskeletonSpaceMatrixは一致する
		{
			joint.skeletonSpaceMatrix = joint.localMatrix;
		}
		//Scale成分を取り除く
		Matrix4x4 jointWorldTransform = joint.skeletonSpaceMatrix;
		for (int i = 0; i < 3; ++i)
		{
			float scale = std::sqrt(jointWorldTransform.m[i][0] * jointWorldTransform.m[i][0] +
				jointWorldTransform.m[i][1] * jointWorldTransform.m[i][1] +
				jointWorldTransform.m[i][2] * jointWorldTransform.m[i][2]);
			for (int j = 0; j < 3; ++j)
			{
				jointWorldTransform.m[i][j] /= scale;
			}
		}
		//ワールド行列を掛ける
		jointWorldTransforms_[joint.index].matWorld_ = jointWorldTransform * worldTransform.matWorld_;
	}
}

void Model::UpdateSkinClusters()
{
	//スキンクラスターの更新
	for (int32_t i = 0; i < meshes_.size(); ++i)
	{
		if (modelData_.skinClusterData[i].empty())
		{
			continue;
		}

		for (size_t jointIndex = 0; jointIndex < skeleton_.joints.size(); ++jointIndex)
		{
			assert(jointIndex < skinClusters_[i].inverseBindPoseMatrices.size());
			skinClusters_[i].mappedPalette[jointIndex].skeletonSpaceMatrix = skinClusters_[i].inverseBindPoseMatrices[jointIndex] * skeleton_.joints[jointIndex].skeletonSpaceMatrix;
			skinClusters_[i].mappedPalette[jointIndex].skeletonSpaceInverseTransposeMatrix = Mathf::Transpose(Mathf::Inverse(skinClusters_[i].mappedPalette[jointIndex].skeletonSpaceMatrix));
		}
	}
}

void Model::UpdateMaterials()
{
	//マテリアルの更新
	for (std::unique_ptr<Material>& material : materials_)
	{
		material->Update();
	}
}

const WorldTransform& Model::GetJointWorldTransform(const std::string& name) const
{
	for (const Joint& joint : skeleton_.joints)
	{
		if (joint.name == name)
		{
			return jointWorldTransforms_[joint.index];
		}
	}
	static WorldTransform emptyWorldTransform{};
	return emptyWorldTransform;
}

void Model::ApplyRootTransform(WorldTransform& worldTransform)
{
	//スキンクラスターを持っていない場合にルートのローカル行列をワールド行列に適用
	if (boneVertices_.empty())
	{
		worldTransform.matWorld_ = modelData_.rootNode.localMatrix * worldTransform.matWorld_;
		worldTransform.TransferMatrix();
	}
}