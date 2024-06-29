#include "ParticleSystem.h"
#include "Engine/Base/GraphicsCore.h"
#include "Engine/Math/MathFunction.h"
#include <numbers>

void ParticleSystem::Initialize()
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

	//ParticleResourceの作成
	particleResource_ = std::make_unique<RWStructuredBuffer>();
	particleResource_->Create(kMaxParticles, sizeof(ParticleCS));

	//FreeCounterResourceの作成
	freeCounterResource_ = std::make_unique<RWStructuredBuffer>();
	freeCounterResource_->Create(1, sizeof(int32_t));

	//EmitterResourceの作成
	emitterResource_ = std::make_unique<StructuredBuffer>();
	emitterResource_->Create(kMaxEmitters, sizeof(EmitterSphere));

	//EmitterInformationResourceの作成
	emitterInformationResource_ = std::make_unique<UploadBuffer>();
	emitterInformationResource_->Create(sizeof(int32_t));

	//AccelerationFieldResourceの作成
	accelerationFieldResource_ = std::make_unique<StructuredBuffer>();
	accelerationFieldResource_->Create(kMaxAccelerationFields, sizeof(AccelerationFieldData));

	//AccelerationFieldInformationResourceの作成
	accelerationFieldInformationResource_ = std::make_unique<UploadBuffer>();
	accelerationFieldInformationResource_->Create(sizeof(int32_t));

	//GravityFieldResourceの作成
	gravityFieldResource_ = std::make_unique<StructuredBuffer>();
	gravityFieldResource_->Create(kMaxGravityFields, sizeof(GravityFieldData));

	//GravityFieldInformationResourceの作成
	gravityFieldInformationResource_ = std::make_unique<UploadBuffer>();
	gravityFieldInformationResource_->Create(sizeof(int32_t));

	//PerViewResourceの作成
	perViewResource_ = std::make_unique<UploadBuffer>();
	perViewResource_->Create(sizeof(PerView));
}

void ParticleSystem::Update()
{
	//AccelerationFieldの更新
	UpdateAccelerationFieldResource();

	//GravityFieldの更新
	UpdateGravityFieldResource();

	//コマンドリストを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

	//Particleを設定
	commandContext->SetComputeDescriptorTable(0, particleResource_->GetUAVHandle());

	//AccelerationFieldを設定
	commandContext->SetComputeDescriptorTable(1, accelerationFieldResource_->GetSRVHandle());

	//GravityFieldを設定
	commandContext->SetComputeDescriptorTable(2, gravityFieldResource_->GetSRVHandle());

	//AccelerationFieldInformationを設定
	commandContext->SetComputeConstantBuffer(3, accelerationFieldInformationResource_->GetGpuVirtualAddress());

	//GravityFieldInformationを設定
	commandContext->SetComputeConstantBuffer(4, gravityFieldInformationResource_->GetGpuVirtualAddress());

	//Dispatch
	commandContext->Dispatch(1, 1, 1);
}

void ParticleSystem::UpdateEmitter()
{
	//Emitterの更新
	UpdateEmitterResource();

	//コマンドリストを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

	//Particleを設定
	commandContext->SetComputeDescriptorTable(0, particleResource_->GetUAVHandle());

	//FreeCounterを設定
	commandContext->SetComputeDescriptorTable(1, freeCounterResource_->GetUAVHandle());

	//Emitterを設定
	commandContext->SetComputeDescriptorTable(2, emitterResource_->GetSRVHandle());

	//EmitterInfomationを設定
	commandContext->SetComputeConstantBuffer(3, emitterInformationResource_->GetGpuVirtualAddress());

	//Dispatch
	commandContext->Dispatch(1, 1, 1);

	//UAVBarierを貼る
	commandContext->InsertUAVBarrier(*particleResource_);
}

void ParticleSystem::Draw(const Camera* camera)
{
	//PerViewResourceの更新
	UpdatePerViewResource(camera);

	//モデルを設定
	Model* currentModel = model_ ? model_ : defaultModel_.get();

	//コマンドリストを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

	//ParticleResourceの状態を遷移
	commandContext->TransitionResource(*particleResource_, D3D12_RESOURCE_STATE_GENERIC_READ);

	//モデルの描画
	for (uint32_t i = 0; i < currentModel->meshes_.size(); ++i)
	{
		//マテリアルのインデックスを取得
		uint32_t materialIndex = currentModel->meshes_[i]->GetMaterialIndex();

		//IndexBufferを設定
		commandContext->SetIndexBuffer(currentModel->GetMesh(i)->GetIndexBufferView());

		//VertexBufferを設定
		commandContext->SetVertexBuffer(currentModel->GetMesh(i)->GetVertexBufferView());

		//形状を設定
		commandContext->SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//Materialを設定
		commandContext->SetConstantBuffer(0, currentModel->GetMaterial(materialIndex)->GetConstantBuffer()->GetGpuVirtualAddress());

		//Particleを設定
		commandContext->SetDescriptorTable(1, particleResource_->GetSRVHandle());

		//PerViewを設定
		commandContext->SetConstantBuffer(2, perViewResource_->GetGpuVirtualAddress());

		//Textureを設定
		commandContext->SetDescriptorTable(3, currentModel->GetMaterial(materialIndex)->GetTexture()->GetSRVHandle());

		//描画
		commandContext->DrawIndexedInstanced(UINT(currentModel->meshes_[i]->GetIndicesSize()), kMaxParticles);
	}

	//ParticleResourceの状態を遷移
	commandContext->TransitionResource(*particleResource_, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
}

void ParticleSystem::Clear()
{
	//エミッターのリストをクリア
	particleEmitters_.clear();

	//加速フィールドをクリア
	accelerationFields_.clear();

	//重力フィールドをクリア
	gravityFields_.clear();
}

void ParticleSystem::AddParticleEmitter(ParticleEmitter* particleEmitter)
{
	if (particleEmitters_.size() >= kMaxEmitters)
	{
		delete particleEmitter;
		return;
	}
	particleEmitters_.push_back(std::unique_ptr<ParticleEmitter>(particleEmitter));
}

ParticleEmitter* ParticleSystem::GetParticleEmitter(const std::string& name)
{
	//エミッターのリストから探す
	for (const std::unique_ptr<ParticleEmitter>& particleEmitter : particleEmitters_)
	{
		if (particleEmitter->GetName() == name)
		{
			return particleEmitter.get();
		}
	}
	//見つからなかったらnullptrを返す
	return nullptr;
}

std::vector<ParticleEmitter*> ParticleSystem::GetParticleEmitters(const std::string& name)
{
	//返却する配列
	std::vector<ParticleEmitter*> particleEmitters{};
	//エミッターをリストから探す
	for (const std::unique_ptr<ParticleEmitter>& particleEmitter : particleEmitters_)
	{
		if (particleEmitter->GetName() == name)
		{
			particleEmitters.push_back(particleEmitter.get());
		}
	}
	return particleEmitters;
}

void ParticleSystem::AddAccelerationField(AccelerationField* accelerationField)
{
	if (accelerationFields_.size() >= kMaxAccelerationFields)
	{
		delete accelerationField;
		return;
	}
	accelerationFields_.push_back(std::unique_ptr<AccelerationField>(accelerationField));
}

AccelerationField* ParticleSystem::GetAccelerationField(const std::string& name)
{
	//加速フィールドのリストから探す
	for (const std::unique_ptr<AccelerationField>& accelerationField : accelerationFields_)
	{
		if (accelerationField->GetName() == name)
		{
			return accelerationField.get();
		}
	}
	//見つからなかったらnullptrを返す
	return nullptr;
}

std::vector<AccelerationField*> ParticleSystem::GetAccelerationFields(const std::string& name)
{
	//返却する配列
	std::vector<AccelerationField*> accelerationFields{};
	//加速フィールドをリストから探す
	for (const std::unique_ptr<AccelerationField>& accelerationField : accelerationFields_)
	{
		if (accelerationField->GetName() == name)
		{
			accelerationFields.push_back(accelerationField.get());
		}
	}
	return accelerationFields;
}

void ParticleSystem::AddGravityField(GravityField* gravityField)
{
	if (gravityFields_.size() >= kMaxGravityFields)
	{
		delete gravityField;
		return;
	}
	gravityFields_.push_back(std::unique_ptr<GravityField>(gravityField));
}

GravityField* ParticleSystem::GetGravityField(const std::string& name)
{
	//重力フィールドのリストから探す
	for (const std::unique_ptr<GravityField>& gravityField : gravityFields_)
	{
		if (gravityField->GetName() == name)
		{
			return gravityField.get();
		}
	}
	//見つからなかったらnullptrを返す
	return nullptr;
}

std::vector<GravityField*> ParticleSystem::GetGravityFields(const std::string& name)
{
	//返却する配列
	std::vector<GravityField*> gravityFields{};
	//重力フィールドをリストから探す
	for (const std::unique_ptr<GravityField>& gravityField : gravityFields_)
	{
		if (gravityField->GetName() == name)
		{
			gravityFields.push_back(gravityField.get());
		}
	}
	return gravityFields;
}

void ParticleSystem::UpdateEmitterResource()
{
	//エミッターの削除
	particleEmitters_.erase(std::remove_if(particleEmitters_.begin(), particleEmitters_.end(),
		[](std::unique_ptr<ParticleEmitter>& particleEmitter) {
			if (particleEmitter->GetIsDead()) {
				particleEmitter.reset();
				return true;
			}
			return false;
		}),
		particleEmitters_.end());

	//ポインタを取得
	EmitterSphere* emitterSphereData = static_cast<EmitterSphere*>(emitterResource_->Map());

	//すべてのEmitterの情報を書き込む
	for (uint32_t i = 0; i < particleEmitters_.size(); ++i)
	{
		//Emitterの更新
		particleEmitters_[i]->Update();

		//Emitterの情報を書き込む
		emitterSphereData[i].translate = particleEmitters_[i]->GetTranslate();
		emitterSphereData[i].radius = particleEmitters_[i]->GetRadius();
		emitterSphereData[i].count = particleEmitters_[i]->GetCount();
		emitterSphereData[i].emit = particleEmitters_[i]->GetEmit();
		emitterSphereData[i].scaleMin = particleEmitters_[i]->GetScaleMin();
		emitterSphereData[i].scaleMax = particleEmitters_[i]->GetScaleMax();
		emitterSphereData[i].velocityMin = particleEmitters_[i]->GetVelocityMin();
		emitterSphereData[i].velocityMax = particleEmitters_[i]->GetVelocityMax();
		emitterSphereData[i].lifeTimeMin = particleEmitters_[i]->GetLifeTimeMin();
		emitterSphereData[i].lifeTimeMax = particleEmitters_[i]->GetLifeTimeMax();
		emitterSphereData[i].colorMin = particleEmitters_[i]->GetColorMin();
		emitterSphereData[i].colorMax = particleEmitters_[i]->GetColorMax();
	}

	//Emitterの数の更新
	int32_t* emitterInformationData = static_cast<int32_t*>(emitterInformationResource_->Map());
	*emitterInformationData = (int32_t)particleEmitters_.size();
	emitterInformationResource_->Unmap();
}

void ParticleSystem::UpdateAccelerationFieldResource()
{
	//加速フィールドの削除
	accelerationFields_.erase(std::remove_if(accelerationFields_.begin(), accelerationFields_.end(),
		[](std::unique_ptr<AccelerationField>& accelerationField) {
			if (accelerationField->GetIsDead()) {
				accelerationField.reset();
				return true;
			}
			return false;
		}),
		accelerationFields_.end());

	//ポインタを取得
	AccelerationFieldData* accelerationFieldData = static_cast<AccelerationFieldData*>(accelerationFieldResource_->Map());

	//すべてのAccelerationFieldの情報を書き込む
	for (uint32_t i = 0; i < accelerationFields_.size(); ++i)
	{
		//AccelerationFieldの更新
		accelerationFields_[i]->Update();

		//AccelerationFieldの情報を書き込む
		accelerationFieldData[i].acceleration = accelerationFields_[i]->GetAcceleration();
		accelerationFieldData[i].translate = accelerationFields_[i]->GetTranslate();
		accelerationFieldData[i].min = accelerationFields_[i]->GetMin();
		accelerationFieldData[i].max = accelerationFields_[i]->GetMax();
	}
	accelerationFieldResource_->Unmap();

	//AccelerationFieldの数の更新
	int32_t* accelerationFieldInformationData = static_cast<int32_t*>(accelerationFieldInformationResource_->Map());
	*accelerationFieldInformationData = (int32_t)accelerationFields_.size();
	accelerationFieldInformationResource_->Unmap();
}

void ParticleSystem::UpdateGravityFieldResource()
{
	//重力フィールドの削除
	gravityFields_.erase(std::remove_if(gravityFields_.begin(), gravityFields_.end(),
		[](std::unique_ptr<GravityField>& gravityField) {
			if (gravityField->GetIsDead()) {
				gravityField.reset();
				return true;
			}
			return false;
		}),
		gravityFields_.end());

	//ポインタを取得
	GravityFieldData* gravityFieldData = static_cast<GravityFieldData*>(gravityFieldResource_->Map());

	//すべてのGravityFieldの情報を書き込む
	for (uint32_t i = 0; i < gravityFields_.size(); ++i)
	{
		//GravityFieldの更新
		gravityFields_[i]->Update();

		//GravityFieldの情報を書き込む
		gravityFieldData[i].translate = gravityFields_[i]->GetTranslate();
		gravityFieldData[i].min = gravityFields_[i]->GetMin();
		gravityFieldData[i].max = gravityFields_[i]->GetMax();
		gravityFieldData[i].strength = gravityFields_[i]->GetStrength();
		gravityFieldData[i].stopDistance = gravityFields_[i]->GetStopDistance();
	}
	gravityFieldResource_->Unmap();

	//GravityFieldの数の更新
	int32_t* gravityFieldInformationData = static_cast<int32_t*>(gravityFieldInformationResource_->Map());
	*gravityFieldInformationData = (int32_t)gravityFields_.size();
	gravityFieldInformationResource_->Unmap();
}

void ParticleSystem::UpdatePerViewResource(const Camera* camera)
{
	//BillBoardMatrixの計算
	Matrix4x4 billboardMatrix = Mathf::MakeIdentity4x4();
	if (isBillboard_)
	{
		Matrix4x4 backToFrontMatrix = Mathf::MakeRotateYMatrix(std::numbers::pi_v<float>);
		Matrix4x4 cameraMatrix = Mathf::MakeAffineMatrix({ 1.0f,1.0f,1.0f }, camera->rotation_, camera->translation_);
		billboardMatrix = backToFrontMatrix * cameraMatrix;
		billboardMatrix.m[3][0] = 0.0f;
		billboardMatrix.m[3][1] = 0.0f;
		billboardMatrix.m[3][2] = 0.0f;
	}

	//リソースに書き込む
	PerView* perViewData = static_cast<PerView*>(perViewResource_->Map());
	perViewData->viewMatrix = camera->matView_;
	perViewData->projectionMatrix = camera->matProjection_;
	perViewData->billboardMatrix = billboardMatrix;
	perViewResource_->Unmap();
}