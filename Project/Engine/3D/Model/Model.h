/**
 * @file Model.h
 * @brief モデルを管理するファイル
 * @author 青木智滉
 * @date
 */

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

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="modelData">モデルデータ</param>
	/// <param name="drawPass">描画の種類</param>
	void Initialize(const ModelData& modelData, const DrawPass drawPass);

	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	void Update(WorldTransform& worldTransform);

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="camera">カメラ</param>
	void Draw(const WorldTransform& worldTransform, const Camera& camera);

	/// <summary>
	/// 再利用処理
	/// </summary>
	void Acquire();

	/// <summary>
	/// 解放処理
	/// </summary>
	void Release();

	/// <summary>
	/// マテリアルの更新
	/// </summary>
	void UpdateMaterials();

	//ボーンを描画するかどうかを取得・設定
	const bool GetIsBoneVisible() const { return isBoneVisible_; };
	void SetIsBoneVisible(const bool isBoneVisible) { isBoneVisible_ = isBoneVisible; };

	//影を反映させるかどうかを取得・設定
	const bool GetCastShadows() const { return castShadows_; };
	void SetCastShadows(const bool castShadows) { castShadows_ = castShadows; };

	//使用しているかどうかを取得・設定
	const bool GetIsInUse() const { return isInUse_; };
	void SetIsInUse(const bool isInUse) { isInUse_ = isInUse; };

	//メッシュの数を取得
	const size_t GetNumMeshes() { return meshes_.size(); };

	//マテリアルの数を取得
	const size_t GetNumMaterials() { return materials_.size(); };

	//メッシュを取得
	Mesh* GetMesh(size_t index) { return meshes_[index].get(); };

	//マテリアルを取得
	Material* GetMaterial(size_t index) { return materials_[index].get(); };

	//スケルトンを取得
	Skeleton& GetSkeleton() { return skeleton_; };

	//Rootのノードを取得
	const Node& GetRootNode() const { return modelData_.rootNode; };

	//ジョイントのワールドトランスフォームを取得
	const WorldTransform& GetJointWorldTransform(const std::string& name) const;

	//ジョイントのワールドトランスフォームをまとめて取得
	const std::vector<WorldTransform>& GetJointWorldTransforms() const { return jointWorldTransforms_; };

private:
	/// <summary>
	/// スケルトンを作成
	/// </summary>
	void CreateSkeleton();

	/// <summary>
	/// ジョイントを作成
	/// </summary>
	/// <param name="node">ノード</param>
	/// <param name="parent">親のインデックス</param>
	/// <param name="joints">ジョイントの配列</param>
	/// <returns>自分のインデックス</returns>
	int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

	/// <summary>
	/// ジョイントのワールドトランスフォームを作成
	/// </summary>
	void CreateJointWorldTransforms();

	/// <summary>
	/// スキンクラスターを作成
	/// </summary>
	void CreateSkinClusters();

	/// <summary>
	/// メッシュを作成
	/// </summary>
	void CreateMeshes();

	/// <summary>
	/// マテリアルを作成
	/// </summary>
	void CreateMaterials();

	/// <summary>
	/// ボーンの作成
	/// </summary>
	void CreateBone();

	/// <summary>
	/// ボーンの頂点データを作成
	/// </summary>
	/// <param name="parentIndex"></param>
	void CreateBoneVertices(int32_t parentIndex);

	/// <summary>
	/// ボーンの頂点バッファを更新
	/// </summary>
	void UpdateBoneVertexData();

	/// <summary>
	/// ボーンの頂点バッファを作成
	/// </summary>
	void CreateBoneVertexBuffer();

	/// <summary>
	/// スケルトンの更新
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	void UpdateSkeleton(const WorldTransform& worldTransform);

	/// <summary>
	/// スキンクラスターの更新
	/// </summary>
	void UpdateSkinClusters();

	/// <summary>
	/// ルートのトランスフォームを適用
	/// </summary>
	/// <param name="worldTransform"></param>
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

