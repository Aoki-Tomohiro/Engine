#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Base/Renderer.h"

class Model
{
public:
	struct ModelData
	{
		std::vector<std::map<std::string, Mesh::JointWeightData>> skinClusterData;
		std::vector<Mesh::MeshData> meshData;
		std::vector<Material::MaterialData> materialData;
		Animation::Node rootNode;
	};

	void Initialize(const ModelData& modelData, const std::vector<Animation::AnimationData>& animationData, const DrawPass drawPass);

	void Update(WorldTransform& worldTransform, const std::string& animationName);

	void Draw(const WorldTransform& worldTransform, const Camera& camera);

	void SetIsBoneVisible(const bool isBoneVisible) { isBoneVisible_ = isBoneVisible; };

	const bool GetCastShadows() const { return castShadows_; };

	void SetCastShadows(const bool castShadows) { castShadows_ = castShadows; };

	Mesh* GetMesh(size_t index) { return meshes_[index].get(); };

	Material* GetMaterial(size_t index) { return materials_[index].get(); };

	Animation* GetAnimation() { return animation_.get(); };

private:
	void CreateBoneVertices(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices);

	void UpdateVertexData(const Animation::Skeleton& skeleton, int32_t parentIndex, std::vector<Vector4>& vertices);

	void CreateBoneVertexBuffer();

private:
	ModelData modelData_{};

	std::vector<std::unique_ptr<Mesh>> meshes_{};

	std::vector<std::unique_ptr<Material>> materials_{};

	std::unique_ptr<Animation> animation_ = nullptr;

	DrawPass drawPass_ = Opaque;

	std::unique_ptr<UploadBuffer> boneVertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW boneVertexBufferView_{};

	std::vector<Vector4> boneVertices_{};

	bool isBoneVisible_ = false;

	bool castShadows_ = true;

	bool hasSkinCluster_ = false;

	friend class ParticleSystem;
};

