#pragma once
#include "WorldTransform.h"
#include "Camera.h"
#include <memory>
#include <string>
#include <vector>
//#include <assimp/Importer.hpp>
//#include <assimp/scene.h>
//#include <assimp/postprocess.h>

enum DrawPass
{
	Opaque,
	Transparent,
	NumTypes,
};

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

	void CreateFromOBJ(const std::string& directoryPath, const std::string& filename, DrawPass drawPass);

	void Update();

	void Draw(const WorldTransform& worldTransform, const Camera& camera);

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

	const Vector2& GetUVScale() const { return uvScale_; };

	void SetUVScale(const Vector2& uvScale) { uvScale_ = uvScale; };

	const float GetUVRotation() const { return uvRotation_; };

	void SetUVRotation(const float uvRotation) { uvRotation_ = uvRotation; };

	const Vector2& GetUVTranslation() const { return uvTranslation_; };

	void SetUVTranslation(const Vector2& uvTranslation) { uvTranslation_ = uvTranslation; };

	const std::string& GetTexture() const { return textureName_; };

	void SetTexture(const std::string& name) { textureName_ = name; };

private:
	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

	void CreateVertexBuffer();

	void CreateMaterialConstBuffer();

	//Model::ModelData LoadModelFile(const std::string& directoryPath, const std::string& filename);

	//Model::Node ReadNode(aiNode* node);

private:
	ModelData modelData_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector2 uvScale_ = { 1.0f,1.0f };

	float uvRotation_ = 0.0f;

	Vector2 uvTranslation_ = { 0.0f,0.0f };

	DrawPass drawPass_ = Opaque;

	std::string textureName_ = "Project/Resources/Images/white.png";

	friend class ParticleSystem;
};

