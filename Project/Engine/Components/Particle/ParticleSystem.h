/**
 * @file ParticleSystem.h
 * @brief パーティクルシステムを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "ParticleEmitter.h"
#include "EmitterBuilder.h"
#include "AccelerationField.h"
#include "GravityField.h"
#include "Engine/Base/RWStructuredBuffer.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/3D/Camera/Camera.h"

class ParticleSystem
{
public:
	//パーティクルの最大数
	static const uint32_t kMaxParticles = 1024;
	//エミッターの最大数
	static const uint32_t kMaxEmitters = 1024;
	//加速フィールドの最大数
	static const uint32_t kMaxAccelerationFields = 10;
	//重力フィールドの最大数
	static const uint32_t kMaxGravityFields = 10;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// エミッターの更新
	/// </summary>
	void UpdateEmitter();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Draw(const Camera* camera);

	/// <summary>
	/// クリア
	/// </summary>
	void Clear();

	/// <summary>
	/// エミッターを追加
	/// </summary>
	/// <param name="particleEmitter">エミッター</param>
	void AddParticleEmitter(ParticleEmitter* particleEmitter);

	/// <summary>
	/// エミッターを削除
	/// </summary>
	/// <param name="name">エミッターの名前</param>
	void RemoveParticleEmitter(const std::string& name);

	/// <summary>
	/// エミッターを取得
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>指定した名前のエミッター</returns>
	ParticleEmitter* GetParticleEmitter(const std::string& name);

	/// <summary>
	/// エミッターまとめて取得
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>指定した名前のエミッターの配列</returns>
	std::vector<ParticleEmitter*> GetParticleEmitters(const std::string& name);

	/// <summary>
	/// 加速フィールドを追加
	/// </summary>
	/// <param name="accelerationField">加速フィールド</param>
	void AddAccelerationField(AccelerationField* accelerationField);

	/// <summary>
	/// 加速フィールドを削除
	/// </summary>
	/// <param name="name">名前</param>
	void RemoveAccelerationField(const std::string& name);

	/// <summary>
	/// 加速フィールドを取得
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>指定した名前の加速フィールド</returns>
	AccelerationField* GetAccelerationField(const std::string& name);

	/// <summary>
	/// 加速フィールドをまとめて取得
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>指定した名前の加速フィールドの配列</returns>
	std::vector<AccelerationField*> GetAccelerationFields(const std::string& name);

	/// <summary>
	/// 重力フィールドを追加
	/// </summary>
	/// <param name="gravityField">重力フィールド</param>
	void AddGravityField(GravityField* gravityField);

	/// <summary>
	/// 重力フィールドを削除
	/// </summary>
	/// <param name="name">名前</param>
	void RemoveGravityField(const std::string& name);

	/// <summary>
	/// 重力フィールドを取得
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>指定した名前の重力フィールド</returns>
	GravityField* GetGravityField(const std::string& name);

	/// <summary>
	/// 重力フィールドをまとめて取得
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>指定した名前の重力フィールドの配列</returns>
	std::vector<GravityField*> GetGravityFields(const std::string& name);

	//テクスチャを設定
	void SetTexture(const std::string& name);

	//モデルを取得・設定
	Model* GetModel()const { return model_; };
	void SetModel(const std::string& name);

	//深度値を書くかどうかを取得・設定
	const bool GetEnableDepthWrite() const { return enableDepthWrite_; };
	void SetEnableDepthWrite(const bool enableDepthWrite) { enableDepthWrite_ = enableDepthWrite; };

	//ブレンドモードを取得・設定
	const BlendMode& GetBlendMode() const { return blendMode_; };
	void SetBlendMode(const BlendMode& blendMode) { blendMode_ = blendMode; };

	//パーティクル用のリソースを取得
	RWStructuredBuffer* GetParticleResource() const { return particleResource_.get(); };

	//空いているパーティクルのインデックス用のリソースを取得
	RWStructuredBuffer* GetFreeListIndexResource() const { return freeListIndexResource_.get(); };

	//空いているリスト用のリソースを取得
	RWStructuredBuffer* GetFreeListResource() const { return freeListResource_.get(); };

private:
	/// <summary>
	/// エミッターのリソースを更新
	/// </summary>
	void UpdateEmitterResource();

	/// <summary>
	/// 加速フィールドのリソースを更新
	/// </summary>
	void UpdateAccelerationFieldResource();

	/// <summary>
	/// 重力フィールドのリソースを更新
	/// </summary>
	void UpdateGravityFieldResource();

	/// <summary>
	/// カメラデータ用のリソースを更新
	/// </summary>
	/// <param name="camera"></param>
	void UpdatePerViewResource(const Camera* camera);

private:
	//ParticleResource
	std::unique_ptr<RWStructuredBuffer> particleResource_ = nullptr;

	//FreeListIndexResource
	std::unique_ptr<RWStructuredBuffer> freeListIndexResource_ = nullptr;

	//FreeListResource
	std::unique_ptr<RWStructuredBuffer> freeListResource_ = nullptr;

	//EmitterResource
	std::unique_ptr<StructuredBuffer> emitterResource_ = nullptr;

	//EmitterInformationResource
	std::unique_ptr<UploadBuffer> emitterInformationResource_ = nullptr;

	//AccelerationFieldResource
	std::unique_ptr<StructuredBuffer> accelerationFieldResource_ = nullptr;

	//AccelerationFieldInformationResource
	std::unique_ptr<UploadBuffer> accelerationFieldInformationResource_ = nullptr;

	//GravityFieldResource
	std::unique_ptr<StructuredBuffer> gravityFieldResource_ = nullptr;

	//GravityFieldInformationResource
	std::unique_ptr<UploadBuffer> gravityFieldInformationResource_ = nullptr;

	//PerViewResource
	std::unique_ptr<UploadBuffer> perViewResource_ = nullptr;

	//モデル
	Model* model_ = nullptr;

	//テクスチャの名前
	std::string textureName_ = "";

	//エミッター
	std::vector<std::unique_ptr<ParticleEmitter>> particleEmitters_{};

	//加速フィールド
	std::vector<std::unique_ptr<AccelerationField>> accelerationFields_{};

	//重力フィールド
	std::vector<std::unique_ptr<GravityField>> gravityFields_{};

	//深度を書くかどうか
	bool enableDepthWrite_ = false;

	//ブレンドモード
	BlendMode blendMode_ = BlendMode::kBlendModeAdd;
};

