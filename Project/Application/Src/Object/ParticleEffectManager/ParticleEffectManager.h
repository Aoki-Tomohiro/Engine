#pragma once
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"

/// <summary>
/// パーティクルエフェクトを管理するクラス
/// </summary>
class ParticleEffectManager
{
public:
	//エミッターの構造体
	struct ParticleEmitterSettings
	{
		Vector3 translate = { 0.0f,0.0f,0.0f };      //位置
		float radius = 1.0f;                         //射出半径
		int32_t count = 10;                          //射出数
		Vector3 rotateMin = { 0.0f,0.0f,0.0f };      //回転角
		Vector3 rotateMax = { 0.0f,0.0f,0.0f };      //回転角
		Vector3 scaleMin = { 1.0f,1.0f,1.0f };       //スケール
		Vector3 scaleMax = { 1.0f,1.0f,1.0f };       //スケール
		Vector3 velocityMin = { -0.2f,-0.2f,-0.2f }; //速度
		Vector3 velocityMax = { 0.2f,0.2f,0.2f };    //速度
		float lifeTimeMin = 0.6f;                    //寿命
		float lifeTimeMax = 1.0f;                    //寿命
		Vector4 colorMin = { 0.0f,0.0f,0.0f,1.0f };  //色
		Vector4 colorMax = { 1.0f,1.0f,1.0f,1.0f };  //色
		float frequency = 0.5f;                      //発射間隔
		float emitterLifeTime = 1.0f;                //エミッターの寿命
		bool alignToDirection = false;               //パーティクルを進行方向に向かせるか
		bool enableColorOverLifeTime = false;        //寿命に変化に応じて色を変えるかどうか
		Vector3 targetColor = { 1.0f,1.0f,1.0f };    //目標の色
		bool enableAlphaOverLifeTime = true;         //寿命の変化に応じて透明度を変えるかどうか
		float targetAlpha = 0.0f;                    //目標のアルファ
		bool enableSizeOverLifeTime = false;         //寿命の変化に応じてサイズを変えるかどうか
		Vector3 targetScale = { 0.0f,0.0f,0.0f };    //目標のサイズ
		bool enableRotationOverLifeTime = false;     //寿命の変化に応じて回転させるかどうか
		Vector3 rotSpeed = { 0.0f,0.0f,0.0f };       //回転速度
	};

	//加速フィールドの構造体
	struct ParticleAccelerationFieldSettings
	{
		Vector3 acceleration_ = { 0.0f,-0.08f,0.0f }; //加速度
		Vector3 translate_ = { 0.0f,0.0f,0.0f };	  //位置
		Vector3 min_ = { -1.0f,-1.0f,-1.0f };         //最小範囲
		Vector3 max_ = { 1.0f,1.0f,1.0f };            //最大範囲
		float deathTimer_ = 0.0f;                     //死亡までのタイマー
	};

	//重力フィールドの構造体
	struct ParticleGravityFieldSettings
	{
		Vector3 translate = { 0.0f,0.0f,0.0f };//位置
		Vector3 min = { -1.0f,-1.0f,-1.0f };   //最小範囲
		Vector3 max = { 1.0f,1.0f,1.0f };      //最大範囲
		float strength = 0.06f;                //重力の強さ
		float stopDistance = 0.4f;             //動きを止める中心点からの距離
		float deathTimer = 0.0f;               //死亡までのタイマー
	};

	//パーティクルシステムの構造体
	struct ParticleSystemSettings
	{
		std::unordered_map<std::string, ParticleEmitterSettings> particleEmitterSettings{};                     //パーティクルのエミッターの設定
		std::unordered_map<std::string, ParticleAccelerationFieldSettings> particleAccelerationFieldSettings{}; //パーティクルの加速フィールドの設定
		std::unordered_map<std::string, ParticleGravityFieldSettings> particleGravityFieldSettings{};           //パーティクルの重力フィールドの設定
	};

	//パーティクルエフェクトの構造体
	struct ParticleEffectSettings
	{
		std::unordered_map<std::string, ParticleSystemSettings> particleSystemSetting{}; //パーティクルエフェクトの設定
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="camera">カメラ</param>
	void Initialize(const Camera* camera);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

private:
	/// <summary>
	/// 新しいパーティクルエフェクトを追加
	/// </summary>
	void AddNewParticleEffect();

	/// <summary>
	/// 名前のバッファをクリアしてリサイズする
	/// </summary>
	void ClearAndResizeBuffer(std::string& buffer);

	/// <summary>
	/// 全てのパーティクルエフェクトの名前を取得
	/// </summary>
	/// <returns>パーティクルエフェクトの名前の配列</returns>
	const std::vector<std::string> GetParticleEffectNames() const;

	/// <summary>
	/// コンボボックスからパーティクルエフェクトを選択
	/// </summary>
	/// <param name="particleEffectNames">パーティクルエフェクトの名前の配列</param>
	void SelectParticleEffect(const std::vector<std::string>& particleEffectNames);

	/// <summary>
	/// 新しいパーティクルシステムの設定を追加
	/// </summary>
	/// <param name="effectSettings">パーティクルエフェクトの設定</param>
	void AddNewParticleSystemSettings(ParticleEffectSettings& effectSettings);

	/// <summary>
	/// パーティクルシステムの設定を編集
	/// </summary>
	/// <param name="effectSettings">パーティクルエフェクトの設定</param> 
	void EditParticleSystemSettings(ParticleEffectSettings& effectSettings);

	/// <summary>
	/// パーティクルエミッターの設定を追加
	/// </summary>
	/// <param name="systemSettings">パーティクルシステムの設定</param> 
	void AddNewParticleEmitterSettings(ParticleSystemSettings& systemSettings);

	/// <summary>
	/// パーティクルエミッターの設定を編集
	/// </summary>
	/// <param name="systemSettings">パーティクルシステムの設定</param> 
	void EditParticleEmitterSettings(ParticleSystemSettings& systemSettings);

	/// <summary>
	/// 新しい加速フィールドの設定を追加
	/// </summary>
	/// <param name="systemSettings">パーティクルシステムの設定</param> 
	void AddNewParticleAccelerationFieldSettings(ParticleSystemSettings& systemSettings);

	/// <summary>
	/// 加速フィールドの設定を編集
	/// </summary>
	/// <param name="systemSettings">パーティクルシステムの設定</param> 
	void EditParticleAccelerationFieldSettings(ParticleSystemSettings& systemSettings);

	/// <summary>
	/// 新しい重力フィールドの設定を追加
	/// </summary>
	/// <param name="systemSettings">パーティクルシステムの設定</param> 
	void AddNewParticleGravityFieldSettings(ParticleSystemSettings& systemSettings);

	/// <summary>
	/// 重力フィールドの設定を編集
	/// </summary>
	/// <param name="systemSettings">パーティクルシステムの設定</param> 
	void EditParticleGravityFieldSettings(ParticleSystemSettings& systemSettings);

private:
	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//パーティクルシステム
	std::map<std::string, ParticleSystem*> particleSystems_{};

	//パーティクルエフェクトの構造体
	std::unordered_map<std::string, ParticleEffectSettings> particleEffectSettings_{};

	//新しく追加するパーティクルエフェクトの設定名
	std::string newParticleEffectSettingsName_ = "";

	//新しく追加するパーティクルシステムの設定名
	std::string newParticleSystemSettingsName_ = "";

	//新しく追加するパーティクルエミッターの設定名
	std::string newParticleEmitterSettingsName_ = "";

	//新しく追加する加速フィールドの設定名
	std::string newParticleAccelerationFieldSettingsName_ = "";

	//新しく追加する重力フィールドの設定名
	std::string newParticleGravityFieldSettingsName_ = "";

	//現在選択されているパーティクルエフェクトの設定のインデックス
	uint32_t currentParticleEffectSettingsIndex_ = 0;
};

