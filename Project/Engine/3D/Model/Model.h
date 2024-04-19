#pragma once
#include "Engine/Base/Renderer.h"
#include "Engine/Base/Texture.h"
#include "Engine/Base/StructuredBuffer.h"
#include "Engine/3D/Camera/Camera.h"
#include "WorldTransform.h"
#include <algorithm>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <optional>
#include <span>

class Model
{
public:
	enum DiffuseReflectionType
	{
		LambertianReflectance,
		HalfLambert,
	};

	enum SpecularReflectionType
	{
		PhongReflectionModel,
		BlinnPhongReflectionModel,
		NoSpecularReflection,
	};

	//ノード構造体
	struct Node {
		Vector3 translate{};
		Quaternion rotate{};
		Vector3 scale{};
		Matrix4x4 localMatrix{};
		std::string name;
		std::vector<Node> children;
	};

	//マテリアルデータ構造体
	struct MaterialData {
		std::string textureFilePath;
	};

	struct VertexWeightData
	{
		float weight;
		uint32_t vertexIndex;
	};

	struct JointWeightData
	{
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

	//モデルデータ構造体
	struct ModelData {
		std::map<std::string, JointWeightData> skinClusterData;
		std::vector<VertexDataPosUVNormal> vertices;
		std::vector<uint32_t> indices;
		MaterialData material;
		Node rootNode;
	};

	//Keyframe構造体
	template <typename tValue>
	struct Keyframe{
		float time;
		tValue value;
	};
	using KeyframeVector3 = Keyframe<Vector3>;
	using KeyframeQuaternion = Keyframe<Quaternion>;

	//AnimationCurve構造体
	template <typename tValue>
	struct AnimationCurve {
		std::vector<Keyframe<tValue>> keyframes;
	};

	//NodeAnimation構造体
	struct NodeAnimation {
		AnimationCurve<Vector3> translate;
		AnimationCurve<Quaternion> rotate;
		AnimationCurve<Vector3> scale;
	};

	//Animation構造体
	struct Animation {
		float duration;//アニメーション全体の尺(単位は秒)
		//NodeAnimationの集合。Node名でひけるようにしておく
		std::map<std::string, NodeAnimation> nodeAnimations;
	};

	struct Joint
	{
		Vector3 translate;//translate
		Quaternion rotate;//rotate
		Vector3 scale;//scale
		Matrix4x4 localMatrix;//localMatrix
		Matrix4x4 skeletonSpaceMatrix;//SkeletonSpaceでの変換行列
		std::string name;//名前
		std::vector<int32_t> children;//子JointのIndexのリスト。いなければ空
		int32_t index;//自身のIndex
		std::optional<int32_t> parent;//親JointのIndex。いなければnull
	};

	struct Skelton
	{
		int32_t root;//RootJointのIndex
		std::map<std::string, int32_t> jointMap;//Joint名とIndexとの辞書
		std::vector<Joint> joints;//所属しているジョイント
	};

	static const uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence
	{
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointIndices;
	};

	struct WellForGPU
	{
		Matrix4x4 skeletonSpaceMatrix;//位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線用
	};

	struct SkinCluster
	{
		std::vector<Matrix4x4> inverseBindPoseMatrices;
		std::unique_ptr<UploadBuffer> influenceResource;
		D3D12_VERTEX_BUFFER_VIEW influenceBufferView;
		std::span<VertexInfluence> mappedInfluence;
		std::unique_ptr<StructuredBuffer> paletteResource;
		std::span<WellForGPU> mappedPalette;
	};

	void Create(const ModelData& modelData, const Animation& animationData, DrawPass drawPass);

	void Update(WorldTransform& worldTransform);

	void ApplyAnimation();

	void Draw(WorldTransform& worldTransform, const Camera& camera);

	const Vector4& GetColor() const { return color_; };

	void SetColor(const Vector4& color) { color_ = color; };

	const Vector2& GetUVScale() const { return uvScale_; };

	void SetUVScale(const Vector2& uvScale) { uvScale_ = uvScale; };

	const float GetUVRotation() const { return uvRotation_; };

	void SetUVRotation(const float uvRotation) { uvRotation_ = uvRotation; };

	const Vector2& GetUVTranslation() const { return uvTranslation_; };

	void SetUVTranslation(const Vector2& uvTranslation) { uvTranslation_ = uvTranslation; };

	const int32_t& GetEnableLighting() const { return enableLighting_; };

	void SetEnableLighting(const int32_t enableLighting) { enableLighting_ = enableLighting; };

	const int32_t& GetDiffuseReflectionType() const { return int32_t(diffuseReflectionType_); };

	void SetDiffuseReflectionType(const int32_t diffuseReflectionType) { diffuseReflectionType_ = DiffuseReflectionType(diffuseReflectionType); };

	const int32_t& GetSpecularReflectionType() const { return int32_t(specularReflectionType_); };

	void SetSpecularReflectionType(const int32_t specularReflectionType) { specularReflectionType_ = SpecularReflectionType(specularReflectionType); };

	const float& GetShininess() const { return shininess_; };

	void SetShininess(const float shininess) { shininess_ = shininess; };

	const Vector3& GetSpecularColor() const { return specularColor_; };

	void SetSpecularColor(const Vector3& specularColor) { specularColor_ = specularColor; };

	void SetTexture(const std::string& textureName);

private:
	void CreateVertexBuffer();

	void CreateIndexBuffer();

	void CreateMaterialConstBuffer();

	void UpdateMaterailConstBuffer();

	Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);

	Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

	Skelton CreateSkeleton(const Node& rootNode);

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	SkinCluster CreateSkinCluster(const Skelton& skeleton, const ModelData& modelData);

private:
	ModelData modelData_{};

	Animation animationData_{};

	Skelton skeletonData_{};

	SkinCluster skinCluster_{};

	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};

	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;

	D3D12_INDEX_BUFFER_VIEW indexBufferView_{};

	std::unique_ptr<UploadBuffer> materialConstBuffer_ = nullptr;

	Vector4 color_ = { 1.0f,1.0f,1.0f,1.0f };

	Vector2 uvScale_ = { 1.0f,1.0f };

	float uvRotation_ = 0.0f;

	Vector2 uvTranslation_ = { 0.0f,0.0f };

	int32_t enableLighting_ = true;

	DiffuseReflectionType diffuseReflectionType_ = DiffuseReflectionType::HalfLambert;

	SpecularReflectionType specularReflectionType_ = SpecularReflectionType::BlinnPhongReflectionModel;

	float shininess_ = 40.0f;

	Vector3 specularColor_ = { 1.0f,1.0f,1.0f };

	DrawPass drawPass_ = Opaque;

	const Texture* texture_ = nullptr;

	float animationTime_ = 0.0f;

	friend class ParticleSystem;
};

