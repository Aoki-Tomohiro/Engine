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

	CreateInstancingResource();
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
	UpdateInstancingResource(camera);
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();
	Model* model = model_ ? model_ : defaultModel_.get();
	commandContext->SetIndexBuffer(model->GetMesh(0)->GetIndexBufferView());
	commandContext->SetVertexBuffer(model->GetMesh(0)->GetVertexBufferView());
	commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandContext->SetConstantBuffer(0, model->GetMaterial(0)->GetConstantBuffer()->GetGpuVirtualAddress());
	commandContext->SetDescriptorTable(1, instancingResource_->GetSRVHandle());
	commandContext->SetConstantBuffer(2, camera.GetConstantBuffer()->GetGpuVirtualAddress());
	commandContext->SetDescriptorTable(3, model->GetMaterial(0)->GetTexture()->GetSRVHandle());
	commandContext->DrawIndexedInstanced(UINT(model->modelData_.meshData[0].indices.size()), numInstance_);
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

void ParticleSystem::CreateInstancingResource()
{
	//Instancing用のWorldTransformリソースを作る
	instancingResource_ = std::make_unique<StructuredBuffer>();
	instancingResource_->Create(kMaxInstance, sizeof(ParticleForGPU));

	//書き込むためのアドレスを取得
	ParticleForGPU* instancingData_ = static_cast<ParticleForGPU*>(instancingResource_->Map());
	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kMaxInstance; ++index)
	{
		instancingData_[index].world = Mathf::MakeIdentity4x4();
		instancingData_[index].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	instancingResource_->Unmap();
}

void ParticleSystem::UpdateInstancingResource(const Camera& camera)
{
	numInstance_ = 0;
	Matrix4x4 billboardMatrix{};
	if (isBillboard_)
	{
		Matrix4x4 backToFrontMatrix = Mathf::MakeRotateYMatrix(std::numbers::pi_v<float>);
		Matrix4x4 cameraMatrix = Mathf::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, camera.rotation_, camera.translation_);
		billboardMatrix = backToFrontMatrix * cameraMatrix;
		billboardMatrix.m[3][0] = 0.0f;
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;
		//billboardMatrix = Mathf::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, camera.rotation_, camera.translation_);;
		//billboardMatrix.m[3][0] = 0.0f;
		//billboardMatrix.m[3][1] = 0.0f;
		//billboardMatrix.m[3][2] = 0.0f;
	}
	ParticleForGPU* instancingData = static_cast<ParticleForGPU*>(instancingResource_->Map());
	for (std::list<std::unique_ptr<ParticleEmitter>>::iterator emitterIterator = particleEmitters_.begin(); emitterIterator != particleEmitters_.end();)
	{
		//パーティクルのリストを取得
		std::list<std::unique_ptr<Particle>>& particles = emitterIterator->get()->GetParticles();
		for (std::list<std::unique_ptr<Particle>>::iterator particleIterator = particles.begin(); particleIterator != particles.end();)
		{
			Matrix4x4 worldMatrix{};
			if (isBillboard_)
			{
				Matrix4x4 scaleMatrix = Mathf::MakeScaleMatrix(particleIterator->get()->GetScale());
				Matrix4x4 translateMatrix = Mathf::MakeTranslateMatrix(particleIterator->get()->GetTranslation());
				worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;
			}
			else
			{
				if (particleIterator->get()->GetRotation() != Vector3{ 0.0f,0.0f,0.0f })
				{
					worldMatrix = Mathf::MakeAffineMatrix(particleIterator->get()->GetScale(), particleIterator->get()->GetRotation(), particleIterator->get()->GetTranslation());
				}
				else
				{
					worldMatrix = Mathf::MakeAffineMatrix(particleIterator->get()->GetScale(), particleIterator->get()->GetQuaternion(), particleIterator->get()->GetTranslation());
				}
			}

			if (numInstance_ < kMaxInstance)
			{
				instancingData[numInstance_].world = worldMatrix;
				instancingData[numInstance_].color = particleIterator->get()->GetColor();
				numInstance_++;
			}
			++particleIterator;
		}
		++emitterIterator;
	}
	instancingResource_->Unmap();
}

void ParticleSystem::Clear()
{
	//エミッターのリストをクリア
	particleEmitters_.clear();
}