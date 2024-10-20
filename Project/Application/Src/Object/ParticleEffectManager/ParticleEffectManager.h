#pragma once
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include <Engine/Externals/nlohmann/json.hpp>
#include <fstream>
#include <numbers>

/// <summary>
/// パーティクルエフェクトを管理するクラス
/// </summary>
class ParticleEffectManager
{
public:
	//エミッターの構造体
	struct EmitterSettings
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
		bool isBillboard = true;                     //ビルボードフラグ
		bool emitIn360Degrees = false;               //360度に放出するかどうか
	};

	//加速フィールドの構造体
	struct AccelerationFieldSettings
	{
		Vector3 acceleration = { 0.0f,-0.08f,0.0f }; //加速度
		Vector3 translate = { 0.0f,0.0f,0.0f };	     //位置
		Vector3 min = { -1.0f,-1.0f,-1.0f };         //最小範囲
		Vector3 max = { 1.0f,1.0f,1.0f };            //最大範囲
		float deathTimer = 0.0f;                     //死亡までのタイマー
	};

	//重力フィールドの構造体
	struct GravityFieldSettings
	{
		Vector3 translate = { 0.0f,0.0f,0.0f }; //位置
		Vector3 min = { -1.0f,-1.0f,-1.0f };    //最小範囲
		Vector3 max = { 1.0f,1.0f,1.0f };       //最大範囲
		float strength = 0.06f;                 //重力の強さ
		float stopDistance = 0.4f;              //動きを止める中心点からの距離
		float deathTimer = 0.0f;                //死亡までのタイマー
	};

	//パーティクルシステムの構造体
	struct ParticleSystemSettings
	{
		std::map<std::string, EmitterSettings> emitterSettings{};                     //パーティクルのエミッターの設定
		std::map<std::string, AccelerationFieldSettings> accelerationFieldSettings{}; //パーティクルの加速フィールドの設定
		std::map<std::string, GravityFieldSettings> gravityFieldSettings{};           //パーティクルの重力フィールドの設定
	};

	//パーティクルエフェクトの構造体
	struct ParticleEffectSettings
	{
		std::map<std::string, ParticleSystemSettings> particleSystemSettings{}; //パーティクルエフェクトの設定
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

	/// <summary>
	/// パーティクルを生成
	/// </summary>
	/// <param name="particleEffectName">パーティクルエフェクトの名前</param>
	/// <param name="position">座標</param>
	/// <param name="quaternion">姿勢</param>
	void CreateParticles(const std::string& particleEffectName, const Vector3& position, const Quaternion& quaternion);

	/// <summary>
	/// エミッターを取得
	/// </summary>
	/// <param name="particleSystemName">パーティクルシステムの名前</param>
	/// <param name="emitterName">エミッターの名前</param>
	/// <returns>エミッター</returns>
	ParticleEmitter* GetEmitter(const std::string& particleSystemName, const std::string& emitterName) const;

	/// <summary>
	/// 加速フィールドを取得
	/// </summary>
	/// <param name="particleSystemName">パーティクルシステムの名前</param>
	/// <param name="accelerationFieldName">加速フィールドの名前</param>
	/// <returns>加速フィールド</returns>
	AccelerationField* GetAccelerationField(const std::string& particleSystemName, const std::string& accelerationFieldName) const;

	/// <summary>
	/// 重力フィールドを取得
	/// </summary>
	/// <param name="particleSystemName">パーティクルシステムの名前</param>
	/// <param name="gravityFieldName">重力フィールドの名前</param>
	/// <returns>重力フィールド</returns>
	GravityField* GetGravityField(const std::string& particleSystemName, const std::string& gravityFieldName) const;

private:
	//保存先ファイルパス
	const std::string kDirectoryPath = "Application/Resources/Config/Particles/";

	/// <summary>
	/// 名前入力用のバッファにサイズを設定
	/// </summary>
	/// <param name="newSize">サイズ</param> 
	void ResizeNameBuffers(const size_t newSize);

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
	/// 設定を保存
	/// </summary>
	/// <param name="particleEffectName">パーティクルエフェクトの名前</param>
	void SaveFile(const std::string& particleEffectName);

	/// <summary>
	/// 全てのパーティクルエフェクトのデータの読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// パーティクルエフェクトのデータの読み込み
	/// </summary>
	/// <param name="particleEffectName">パーティクルエフェクトの名前</param>
	void LoadFile(const std::string& particleEffectName);

	/// <summary>
	/// エミッターのデータを読み込む
	/// </summary>
	/// <param name="particleSystemSettings">パーティクルシステムの設定</param>
	/// <param name="systemData">パーティクルシステムのデータ</param>
	void SetEmitterSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData);

	/// <summary>
	/// 加速フィールドのデータを読み込む
	/// </summary>
	/// <param name="particleSystemSettings">パーティクルシステムの設定</param>
	/// <param name="systemData">パーティクルシステムのデータ</param>
	void SetAccelerationFieldSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData);

	/// <summary>
	/// 重力フィールドのデータを読み込む
	/// </summary>
	/// <param name="particleSystemSettings">パーティクルシステムの設定</param>
	/// <param name="systemData">パーティクルシステムのデータ</param>
	void SetGravityFieldSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData);

	/// <summary>
	/// 新しいパーティクルシステムの設定を追加
	/// </summary>
	/// <param name="effectSettings">パーティクルエフェクトの設定</param>
	void AddNewParticleSystemSettings(ParticleEffectSettings& effectSettings);

	/// <summary>
	/// 全てのパーティクルシステムの名前を取得
	/// </summary>
	/// <returns>パーティクルエフェクトの名前の配列</returns>
	const std::vector<std::string> GetParticleSystemNames() const;

	/// <summary>
	/// コンボボックスから新しいパーティクルシステムを選択
	/// </summary>
	/// <param name="particleEffectNames">パーティクルシステムの名前の配列</param>
	void SelectNewParticleSystem(const std::vector<std::string>& particleEffectNames);

	/// <summary>
	/// パーティクルシステムの設定を編集
	/// </summary>
	/// <param name="effectSettings">パーティクルエフェクトの設定</param> 
	void EditParticleSystemSettings(ParticleEffectSettings& effectSettings);

	/// <summary>
	/// 現在編集中のパーティクルシステムを選択
	/// </summary>
	/// <param name="effectSettings">パーティクルエフェクトの設定</param> 
	void SelectEditParticleSystem(ParticleEffectSettings& effectSettings);

	/// <summary>
	/// 現在編集中のパーティクルシステムを削除
	/// </summary>
	/// <param name="effectSettings">パーティクルエフェクトの設定</param> 
	void RemoveEditParticleSystem(ParticleEffectSettings& effectSettings);

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

	/// <summary>
	/// エミッターを作成
	/// </summary>
	/// <param name="particleSystemName">パーティクルシステムの名前</param> 
	/// <param name="particleSystemSettings">パーティクルシステムの設定</param> 
	/// <param name="position">座標</param> 
	/// <param name="quaternion">姿勢</param> 
	void CreateEmitters(const std::string& particleSystemName, const ParticleSystemSettings& particleSystemSettings, const Vector3& position, const Quaternion& quaternion);

private:
	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//パーティクルシステム
	std::map<std::string, ParticleSystem*> particleSystems_{};

	//パーティクルエフェクトの構造体
	std::map<std::string, ParticleEffectSettings> particleEffectSettings_{};

	//新しく追加するパーティクルエフェクトの設定名
	std::string newParticleEffectSettingsName_ = "";

	//新しく追加するパーティクルエミッターの設定名
	std::string newEmitterSettingsName_ = "";

	//新しく追加する加速フィールドの設定名
	std::string newAccelerationFieldSettingsName_ = "";

	//新しく追加する重力フィールドの設定名
	std::string newGravityFieldSettingsName_ = "";

	//現在選択されているパーティクルエフェクトの設定のインデックス
	uint32_t currentParticleEffectSettingsIndex_ = 0;

	//現在選択されているパーティクルシステムのインデックス
	uint32_t currentParticleSystemSettingsIndex_ = 0;

	//現在選択しているパーティクルシステムの名前
	std::string selectedParticleSystem_ = "";
};

