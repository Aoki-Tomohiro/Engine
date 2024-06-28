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

	//PerViewResourceの作成
	perViewResource_ = std::make_unique<UploadBuffer>();
	perViewResource_->Create(sizeof(PerView));
}

void ParticleSystem::Update()
{

}

void ParticleSystem::UpdateEmitter()
{

}

void ParticleSystem::Draw(const Camera* camera)
{
	//PerViewResourceの更新
	UpdatePerViewResource(camera);

	//モデルを設定
	Model* currentModel = model_ ? model_ : defaultModel_.get();

	//コマンドリストを取得
	CommandContext* commandContext = GraphicsCore::GetInstance()->GetCommandContext();

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
}

void ParticleSystem::Clear()
{

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