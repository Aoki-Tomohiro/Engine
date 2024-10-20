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

	void Initialize();

	void Update();

	void UpdateEmitter();

	void Draw(const Camera* camera);

	void Clear();

	void AddParticleEmitter(ParticleEmitter* particleEmitter);

	void RemoveParticleEmitter(const std::string& name);

	ParticleEmitter* GetParticleEmitter(const std::string& name);

	std::vector<ParticleEmitter*> GetParticleEmitters(const std::string& name);

	void AddAccelerationField(AccelerationField* accelerationField);

	void RemoveAccelerationField(const std::string& name);

	AccelerationField* GetAccelerationField(const std::string& name);

	std::vector<AccelerationField*> GetAccelerationFields(const std::string& name);

	void AddGravityField(GravityField* gravityField);

	void RemoveGravityField(const std::string& name);

	GravityField* GetGravityField(const std::string& name);

	std::vector<GravityField*> GetGravityFields(const std::string& name);

	void SetTexture(const std::string& name);

	Model* GetModel()const { return model_; };

	void SetModel(const std::string& name);

	const bool GetEnableDepthWrite() const { return enableDepthWrite_; };

	void SetEnableDepthWrite(const bool enableDepthWrite) { enableDepthWrite_ = enableDepthWrite; };

	const BlendMode& GetBlendMode() const { return blendMode_; };

	void SetBlendMode(const BlendMode& blendMode) { blendMode_ = blendMode; };

	RWStructuredBuffer* GetParticleResource() const { return particleResource_.get(); };

	RWStructuredBuffer* GetFreeListIndexResource() const { return freeListIndexResource_.get(); };

	RWStructuredBuffer* GetFreeListResource() const { return freeListResource_.get(); };

private:
	void UpdateEmitterResource();

	void UpdateAccelerationFieldResource();

	void UpdateGravityFieldResource();

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

