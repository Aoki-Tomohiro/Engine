#include "ParticleSystem.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/3D/ModelManager.h"
#include "Engine/Math/MathFunction.h"
#include <numbers>

void ParticleSystem::Initialize()
{
	if (!model_)
	{
		model_ = ModelManager::CreateFromOBJ("Plane", Transparent);
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
	TextureManager* textureManager = TextureManager::GetInstance();
	commandContext->SetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, textureManager->GetDescriptorHeap()->GetDescriptorHeap());
	commandContext->SetVertexBuffer(model_->vertexBufferView_);
	commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandContext->SetConstantBuffer(0, model_->materialConstBuffer_->GetGpuVirtualAddress());
	commandContext->SetDescriptorTable(1, descriptorHandle_);
	commandContext->SetConstantBuffer(2, camera.GetConstantBuffer()->GetGpuVirtualAddress());
	commandContext->SetDescriptorTable(3, textureManager->GetDescriptorHandle(model_->textureName_));
	commandContext->DrawInstanced(UINT(model_->modelData_.vertices.size()), numInstance_);
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

	//Descriptorをコピー
	ID3D12Device* device = GraphicsCore::GetInstance()->GetDevice();
	descriptorHandle_ = TextureManager::GetInstance()->GetDescriptorHeap()->Allocate();
	device->CopyDescriptorsSimple(1, descriptorHandle_, instancingResource_->GetSRV(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

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
	Matrix4x4 backToFrontMatrix = Mathf::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 cameraMatrix = Mathf::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, camera.rotation_, camera.translation_);
	Matrix4x4 billboardMatrix = backToFrontMatrix * cameraMatrix;
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;
	ParticleForGPU* instancingData = static_cast<ParticleForGPU*>(instancingResource_->Map());
	for (std::list<std::unique_ptr<ParticleEmitter>>::iterator emitterIterator = particleEmitters_.begin(); emitterIterator != particleEmitters_.end();)
	{
		//パーティクルのリストを取得
		std::list<std::unique_ptr<Particle>>& particles = emitterIterator->get()->GetParticles();
		for (std::list<std::unique_ptr<Particle>>::iterator particleIterator = particles.begin(); particleIterator != particles.end();)
		{
			Matrix4x4 scaleMatrix = Mathf::MakeScaleMatrix(particleIterator->get()->GetScale());
			Matrix4x4 translateMatrix = Mathf::MakeTranslateMatrix(particleIterator->get()->GetTranslation());
			Matrix4x4 worldMatrix{};
			if (isBillboard_)
			{
				worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;
			}
			else
			{
				Matrix4x4 rotateXMatrix = Mathf::MakeRotateXMatrix(particleIterator->get()->GetRotation().x);
				Matrix4x4 rotateYMatrix = Mathf::MakeRotateXMatrix(particleIterator->get()->GetRotation().y);
				Matrix4x4 rotateZMatrix = Mathf::MakeRotateXMatrix(particleIterator->get()->GetRotation().z);
				Matrix4x4 rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
				worldMatrix = scaleMatrix * rotateMatrix * translateMatrix;
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