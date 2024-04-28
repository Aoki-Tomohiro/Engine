#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Animation.h"
#include "WorldTransform.h"
#include "Engine/Base/Renderer.h"
#include "Engine/3D/Camera/Camera.h"

class Model
{
public:
	//モデルデータ構造体
	struct ModelData {
		std::vector<VertexDataPosUVNormal> vertices;
		std::vector<uint32_t> indices;
		Material::MaterialData material;
		Animation::Node rootNode;
	};

	void Create(const ModelData& modelData, const Animation::AnimationData& animationData, DrawPass drawPass);

	void Update(WorldTransform& worldTransform);

	void Draw(WorldTransform& worldTransform, const Camera& camera);

	Mesh* GetMesh() { return mesh_.get(); };

	Material* GetMaterial() { return material_.get(); };

	Animation* GetAnimation() { return animation_.get(); };

private:
	ModelData modelData_{};

	std::unique_ptr<Mesh> mesh_ = nullptr;

	std::unique_ptr<Material> material_ = nullptr;

	std::unique_ptr<Animation> animation_ = nullptr;

	DrawPass drawPass_ = Opaque;

	friend class ParticleSystem;
};

