#include "ParticleSystem.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Math/MathFunction.h"
#include <numbers>

void ParticleSystem::Initialize()
{
	if (!model_)
	{
		//モデルデータ
		Model::ModelData modelData{};

		//メッシュデータ
		Mesh::MeshData meshData{};
		meshData.vertices.resize(4);
		meshData.vertices[0] = { {-1.0f,1.0f,0.0f,1.0f},{0.0f,1.0f},{-1.0f,-1.0f,-1.0f} };
		meshData.vertices[1] = { {-1.0f,-1.0f,0.0f,1.0f},{0.0f,0.0f},{-1.0f,-1.0f,-1.0f} };
		meshData.vertices[2] = { {1.0f,1.0f,0.0f,1.0f},{1.0f,1.0f},{-1.0f,-1.0f,-1.0f} };
		meshData.vertices[3] = { {1.0f,-1.0f,0.0f,1.0f},{1.0f,0.0f},{-1.0f,-1.0f,-1.0f} };
		meshData.indices.resize(6);
		meshData.indices[0] = 0;
		meshData.indices[1] = 1;
		meshData.indices[2] = 2;
		meshData.indices[3] = 1;
		meshData.indices[4] = 3;
		meshData.indices[5] = 2;
		meshData.materialIndex = 0;
		modelData.skinClusterData.push_back(std::map<std::string, Mesh::JointWeightData>());
		modelData.meshData.push_back(meshData);

		//マテリアルデータ
		modelData.materialData.push_back(Material::MaterialData());

		//アニメーションデータ
		std::vector<Animation::AnimationData> animationData(1);

		//モデルの作成
		defaultModel_ = std::make_unique<Model>();
		defaultModel_->Initialize(modelData, animationData, Transparent);
		defaultModel_->GetMaterial(0)->SetTexture("DefaultParticle.png");
	}

	//Instancing用のリソースを作る
	instancingResource_ = std::make_unique<RWStructuredBuffer>();
	instancingResource_->Create(kMaxInstance, sizeof(ParticleCS));

	//PerView用のリソースを作る
	CreatePerViewResource();
}

void ParticleSystem::Update()
{
	//エミッターの削除
	particleEmitters_.remove_if([](std::unique_ptr<ParticleEmitter>& particleEmitter)
		{
			if (particleEmitter->GetIsDead())
			{
				particleEmitter.reset();
				return true;
			}
			return false;
		}
	);

	//エミッターの更新
	for (std::unique_ptr<ParticleEmitter>& emitter : particleEmitters_)
	{
		emitter->Update();
	}
}

void ParticleSystem::Draw(const Camera& camera)
{
	UpdatePerViewResource(camera);
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();
	Model* model = model_ ? model_ : defaultModel_.get();
	commandContext->SetIndexBuffer(model->GetMesh(0)->GetIndexBufferView());
	commandContext->SetVertexBuffer(model->GetMesh(0)->GetVertexBufferView());
	commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandContext->SetConstantBuffer(0, model->GetMaterial(0)->GetConstantBuffer()->GetGpuVirtualAddress());
	commandContext->SetDescriptorTable(1, instancingResource_->GetSRVHandle());
	commandContext->SetConstantBuffer(2, perViewResource_->GetGpuVirtualAddress());
	commandContext->SetDescriptorTable(3, model->GetMaterial(0)->GetTexture()->GetSRVHandle());
	commandContext->DrawIndexedInstanced(UINT(model->modelData_.meshData[0].indices.size()), kMaxInstance);
}

ParticleEmitter* ParticleSystem::GetParticleEmitter(const std::string& name)
{
	//エミッターのリストから探す
	for (std::unique_ptr<ParticleEmitter>& particleEmitter : particleEmitters_)
	{
		if (particleEmitter->GetName() == name)
		{
			return particleEmitter.get();
		}
	}
	//見つからなかったらnullptrを返す
	return nullptr;
}

std::list<ParticleEmitter*> ParticleSystem::GetParticleEmitters(const std::string& name)
{
	//返却するリスト
	std::list<ParticleEmitter*> particleEmitters{};
	//エミッターをリストから探す
	for (std::unique_ptr<ParticleEmitter>& particleEmitter : particleEmitters_)
	{
		if (particleEmitter->GetName() == name)
		{
			particleEmitters.push_back(particleEmitter.get());
		}
	}
	return particleEmitters;
}

void ParticleSystem::CreatePerViewResource()
{
	perViewResource_ = std::make_unique<UploadBuffer>();
	perViewResource_->Create(sizeof(PerView));

	PerView* perViewData = static_cast<PerView*>(perViewResource_->Map());
	perViewData->viewMatrix = Mathf::MakeIdentity4x4();
	perViewData->projectionMatrix = Mathf::MakeIdentity4x4();
	perViewData->billboardMatrix = Mathf::MakeIdentity4x4();
	perViewResource_->Unmap();
}

void ParticleSystem::UpdatePerViewResource(const Camera& camera)
{
	//BillBoardMatrixの計算
	Matrix4x4 backToFrontMatrix = Mathf::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 cameraMatrix = Mathf::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, camera.rotation_, camera.translation_);
	Matrix4x4 billboardMatrix = backToFrontMatrix * cameraMatrix;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	//リソースに書き込む
	PerView* perViewData = static_cast<PerView*>(perViewResource_->Map());
	perViewData->viewMatrix = camera.matView_;
	perViewData->projectionMatrix = camera.matProjection_;
	perViewData->billboardMatrix = billboardMatrix;
	perViewResource_->Unmap();
}

void ParticleSystem::Clear()
{
	//エミッターのリストをクリア
	particleEmitters_.clear();
}