/**
 * @file ParticleManager.h
 * @brief 全てのパーティクルシステムを管理するクラス
 * @author 青木智滉
 * @date
 */

#pragma once
#include "ParticleSystem.h"
#include "Engine/Base/GraphicsPSO.h"
#include "Engine/Base/ComputePSO.h"
#include <map>
#include <string>
#include <algorithm>

class ParticleManager
{
public:
	/// <summary>
	/// インスタンスを取得
	/// </summary>
	/// <returns>インスタンス</returns>
	static ParticleManager* GetInstance();

	/// <summary>
	/// 破棄処理
	/// </summary>
	static void Destroy();

	/// <summary>
	/// パーティクルシステムを生成
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>パーティクルシステム</returns>
	static ParticleSystem* Create(const std::string& name);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// クリア
	/// </summary>
	void Clear();

	/// <summary>
	/// カメラを設定
	/// </summary>
	/// <param name="camera">カメラ</param>
	void SetCamera(const Camera* camera) { camera_ = camera; };

private:
	ParticleManager() = default;
	~ParticleManager() = default;
	ParticleManager(const ParticleManager&) = delete;
	ParticleManager& operator=(const ParticleManager&) = delete;

	/// <summary>
	/// パイプラインステートを生成
	/// </summary>
	void CreateParticlePipelineState();

	/// <summary>
	/// 初期化用のパイプラインステートを生成
	/// </summary>
	void CreateInitializeParticlePipelineState();

	/// <summary>
	/// パーティクルを生成用のパイプラインステートを生成
	/// </summary>
	void CreateEmitParticlePipelineState();

	/// <summary>
	/// パーティクルの更新用のパイプラインステートを生成
	/// </summary>
	void CreateUpdateParticlePipelineState();

	/// <summary>
	/// パーティクルシステムを内部で生成
	/// </summary>
	/// <param name="name">名前</param>
	/// <returns>パーティクルシステム</returns>
	ParticleSystem* CreateInternal(const std::string& name);

private:
	static ParticleManager* instance_;

	std::map<std::string, std::unique_ptr<ParticleSystem>> particleSystems_{};

	std::unique_ptr<UploadBuffer> perFrameResource_ = nullptr;

	const Camera* camera_ = nullptr;

	RootSignature particleRootSignature_{};

	RootSignature initializeParticleRootSignature_{};

	RootSignature emitParticleRootSignature_{};

	RootSignature updateParticleRootSignature_{};

	std::vector<std::vector<GraphicsPSO>> particlePipelineStates_{};

	ComputePSO initializeParticlePipelineState_{};

	ComputePSO emitParticlePipelineState_{};

	ComputePSO updateParticlePipelineState_{};
};

