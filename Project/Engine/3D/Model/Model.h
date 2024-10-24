#pragma once
#include "Mesh.h"
#include "Material.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/3D/Camera/Camera.h"
#include "Engine/Base/Renderer.h"
#include <array>
#include <map>
#include <optional>

class Model
{
public:
	//ノードの構造体
	struct Node
	{
		Vector3 scale;
		Quaternion rotate;
		Vector3 translate;
		Matrix4x4 localMatrix{};
		std::string name;
		std::vector<Node> children;
	};

	//ジョイントの構造体
	struct Joint
	{
		Vector3 scale;//scale
		Quaternion rotate;//rotate
		Vector3 translate;//translate
		Matrix4x4 localMatrix;//LocalMatrix
		Matrix4x4 skeletonSpaceMatrix;//SkeletonSpaceでの変換行列
		std::string name;//名前
		std::vector<int32_t> children;//子JointのIndexのリスト。いなければ空
		int32_t index;//自身のIndex
		std::optional<int32_t> parent;//親JointのIndex。いなければnull
	};

	//スケルトン構造体
	struct Skeleton
	{
		int32_t root;//RootJointのIndex
		std::map<std::string, int32_t> jointMap;//Joint名とIndexとの辞書
		std::vector<Joint> joints;//所属しているジョイント

		void Reset()
		{
			root = -1;
			jointMap.clear();
			joints.clear();
		}
	};

	//頂点のウェイトデータを保持する構造体
	struct VertexWeightData
	{
		float weight;
		uint32_t vertexIndex;
	};

	//ジョイントごとのウェイトデータを保持する構造体
	struct JointWeightData
	{
		Matrix4x4 inverseBindPoseMatrix;
		std::vector<VertexWeightData> vertexWeights;
	};

	//頂点の影響を保持する構造体
	static const uint32_t kNumMaxInfluence = 4;
	struct VertexInfluence
	{
		std::array<float, kNumMaxInfluence> weights;
		std::array<int32_t, kNumMaxInfluence> jointIndices;
	};

	//GPU用の行列データ構造体
	struct WellForGPU
	{
		Matrix4x4 skeletonSpaceMatrix;//位置用
		Matrix4x4 skeletonSpaceInverseTransposeMatrix;//法線用
	};

	//スキンクラスターのデータをまとめた構造体
	struct SkinCluster
	{
		std::vector<Matrix4x4> inverseBindPoseMatrices;
		//Influence
		std::unique_ptr<StructuredBuffer> influenceResource;
		std::span<VertexInfluence> mappedInfluence;
		//MatrixPalette
		std::unique_ptr<StructuredBuffer> paletteResource;
		std::span<WellForGPU> mappedPalette;
	};

	//モデルデータをまとめた構造体
	struct ModelData
	{
		std::vector<std::map<std::string, JointWeightData>> skinClusterData;
		std::vector<Mesh::MeshData> meshData;
		std::vector<Material::MaterialData> materialData;
		Node rootNode;
	};

	void Initialize(const ModelData& modelData, const DrawPass drawPass);

	void Update(WorldTransform& worldTransform);

	void Draw(const WorldTransform& worldTransform, const Camera& camera);

	void Acquire();

	void Release();

	void UpdateMaterials();

	const bool GetIsBoneVisible() const { return isBoneVisible_; };

	void SetIsBoneVisible(const bool isBoneVisible) { isBoneVisible_ = isBoneVisible; };

	const bool GetCastShadows() const { return castShadows_; };

	void SetCastShadows(const bool castShadows) { castShadows_ = castShadows; };

	const bool GetIsInUse() const { return isInUse_; };

	void SetIsInUse(const bool isInUse) { isInUse_ = isInUse; };

	const size_t GetNumMeshes() { return meshes_.size(); };

	const size_t GetNumMaterials() { return materials_.size(); };

	Mesh* GetMesh(size_t index) { return meshes_[index].get(); };

	Material* GetMaterial(size_t index) { return materials_[index].get(); };

	Skeleton& GetSkeleton() { return skeleton_; };

	const Node& GetRootNode() const { return modelData_.rootNode; };

	const WorldTransform& GetJointWorldTransform(const std::string& name) const;

	const std::vector<WorldTransform>& GetJointWorldTransforms() const { return jointWorldTransforms_; };

private:
	void CreateSkeleton();

	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	void CreateJointWorldTransforms();

	void CreateSkinClusters();

	void CreateMeshes();

	void CreateMaterials();

	void CreateBone();

	void CreateBoneVertices(int32_t parentIndex);

	void UpdateBoneVertexData();

	void CreateBoneVertexBuffer();

	void UpdateSkeleton(const WorldTransform& worldTransform);

	void UpdateSkinClusters();

	void ApplyRootTransform(WorldTransform& worldTransform);

private:
	//モデルデータ
	ModelData modelData_{};

	//スケルトン
	Skeleton skeleton_{};

	//スキンクラスター
	std::vector<SkinCluster> skinClusters_{};

	//メッシュ
	std::vector<std::unique_ptr<Mesh>> meshes_{};

	//マテリアル
	std::vector<std::unique_ptr<Material>> materials_{};

	//ボーンの頂点データ
	std::vector<Vector4> boneVertices_{};

	//ボーンの頂点バッファ
	std::unique_ptr<UploadBuffer> boneVertexBuffer_ = nullptr;

	//ボーンの頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW boneVertexBufferView_{};

	//ジョイントのワールドトランスフォーム
	std::vector<WorldTransform> jointWorldTransforms_{};

	//描画パス
	DrawPass drawPass_ = Opaque;

	//影を描画するかどうか
	bool castShadows_ = true;

	//ボーンを描画するかどうか
	bool isBoneVisible_ = false;

	//現在使われているかどうか
	bool isInUse_ = false;
};

