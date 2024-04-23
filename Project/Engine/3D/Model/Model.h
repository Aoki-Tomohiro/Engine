#pragma once
#include "Mesh.h"
#include "Material.h"
#include "WorldTransform.h"
#include "Engine/Base/Renderer.h"
#include "Engine/3D/Camera/Camera.h"

class Model
{
public:
	//ノード構造体
	struct Node {
		Matrix4x4 localMatrix{};
		std::string name;
		std::vector<Node> children;
	};

	//マテリアルデータ構造体
	struct MaterialData {
		std::string textureFilePath;
	};

	//モデルデータ構造体
	struct ModelData {
		std::vector<VertexDataPosUVNormal> vertices;
		MaterialData material;
		Node rootNode;
	};

	void Create(const ModelData& modelData, DrawPass drawPass);

	void Update();

	void Draw(WorldTransform& worldTransform, const Camera& camera);

	Mesh* GetMesh() { return mesh_.get(); };

	Material* GetMaterial() { return material_.get(); };

private:
	ModelData modelData_{};

	std::unique_ptr<Mesh> mesh_ = nullptr;

	std::unique_ptr<Material> material_ = nullptr;

	DrawPass drawPass_ = Opaque;

	friend class ParticleSystem;
};

