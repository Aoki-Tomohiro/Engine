/**
 * @file EditorManager.h
 * @brief パーティクルエフェクトを編集・管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/3D/Camera/CameraManager.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include <Engine/Externals/nlohmann/json.hpp>
#include <fstream>

class ParticleEffectEditor
{
public:
	//エミッターの構造体
	struct EmitterSettings
	{
		bool followEnabled = false;                  //追尾させるかどうか
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
		std::map<std::string, EmitterSettings> emitters{};                     //エミッターの設定
		std::map<std::string, AccelerationFieldSettings> accelerationFields{}; //加速フィールドの設定
		std::map<std::string, GravityFieldSettings> gravityFields{};           //重力フィールドの設定
	};

	//パーティクルエフェクトの構造体
	struct ParticleEffectConfig
	{
		std::map<std::string, ParticleSystemSettings> particleSystems{}; //パーティクルシステムの設定
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

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

	//全てのパーティクルエフェクトの設定を取得
	const std::map<std::string, ParticleEffectConfig>& GetParticleEffectConfigs() const { return particleEffectConfigs_; };

private:
	//保存先ファイルパス
	const std::string kDirectoryPath = "Application/Resources/Config/Particles/";

	/// <summary>
	/// 全てのテクスチャの読み込み
	/// </summary>
	void LoadTextures();

	/// <summary>
	/// 全てのパーティクルシステムの作成
	/// </summary>
	void CreateParticleSystems();

	/// <summary>
	/// 板ポリのパーティクルシステムの作成
	/// </summary>
	/// <param name="systemName">パーティクルシステムの名前</param>
	/// <param name="textureName">テクスチャの名前</param>
	/// <param name="enableDepthWrite">深度を書くかどうか</param>
	void CreateParticleSystemForPlane(const std::string& systemName, const std::string& textureName, const bool enableDepthWrite  = false);

	/// <summary>
	/// メッシュのパーティクルシステムの作成
	/// </summary>
	/// <param name="systemName">パーティクルシステムの名前</param>
	/// <param name="modelName">モデルの名前</param>
	/// <param name="enableDepthWrite">深度を書くかどうか</param>
	/// <param name="enableLighting">ライティングするかどうか</param>
	void CreateParticleSystemForMesh(const std::string& systemName, const std::string& modelName, const bool enableDepthWrite = true, const bool enableLighting = true);

	/// <summary>
	/// 新しいパーティクルエフェクトの設定を追加
	/// </summary>
	void AddNewParticleEffectSetting();

	/// <summary>
	/// 選択されたパーティクルエフェクトのコントロールを表示
	/// </summary>
	/// <param name="effectName">パーティクルエフェクトの名前</param>
	void DisplayEffectControls(const std::string& effectName);

	/// <summary>
	/// ファイルに保存
	/// </summary>
	/// <param name="particleEffectName">パーティクルエフェクトの名前</param>
	void SaveFile(const std::string& particleEffectName);

	/// <summary>
	/// 全てのデータの読み込み
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// データの読み込み
	/// </summary>
	/// <param name="particleEffectName">パーティクルエフェクトの名前</param>
	void LoadFile(const std::string& particleEffectName);

	/// <summary>
	/// エミッターを設定
	/// </summary>
	/// <param name="particleSystemSettings">パーティクルシステムの設定</param>
	/// <param name="systemData">パーティクルシステムのデータ</param>
	void SetEmitterSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData);

	/// <summary>
	/// 加速フィールドを設定
	/// </summary>
	/// <param name="particleSystemSettings">パーティクルシステムの設定</param>
	/// <param name="systemData">パーティクルシステムのデータ</param>
	void SetAccelerationFieldSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData);

	/// <summary>
	/// 重力フィールドを設定
	/// </summary>
	/// <param name="particleSystemSettings">パーティクルシステムの設定</param>
	/// <param name="systemData">パーティクルシステムのデータ</param>
	void SetGravityFieldSettings(ParticleSystemSettings& particleSystemSettings, nlohmann::json& systemData);

	/// <summary>
	/// パーティクルシステムの設定を追加
	/// </summary>
	/// <param name="selectedEffect">選択中のパーティクルエフェクト</param>
	void AddParticleSystemSetting(ParticleEffectConfig& selectedEffect);

	/// <summary>
	/// パーティクルシステムの設定を編集
	/// </summary>
	/// <param name="selectedEffect">選択中のパーティクルエフェクト</param>
	void EditParticleSystemSettings(ParticleEffectConfig& selectedEffect);

	/// <summary>
	/// エミッターの設定を追加
	/// </summary>
	/// <param name="selectedSystem">選択中のパーティクルシステム</param>
	void AddEmitterSetting(ParticleSystemSettings& selectedSystem);

	/// <summary>
	/// エミッターの設定を編集
	/// </summary>
	/// <param name="selectedSystem">選択中のパーティクルシステム</param>
	void EditEmitterSettings(ParticleSystemSettings& selectedSystem);

	/// <summary>
	/// 加速フィールドの設定を追加
	/// </summary>
	/// <param name="selectedSystem">選択中のパーティクルシステム</param>
	void AddAccelerationFieldSetting(ParticleSystemSettings& selectedSystem);

	/// <summary>
	/// 加速フィールドの設定を編集
	/// </summary>
	/// <param name="selectedSystem">選択中のパーティクルシステム</param>
	void EditAccelerationFieldSettings(ParticleSystemSettings& selectedSystem);

	/// <summary>
	/// 重力フィールドの設定を追加
	/// </summary>
	/// <param name="selectedSystem">選択中のパーティクルシステム</param>
	void AddGravityFieldSetting(ParticleSystemSettings& selectedSystem);

	/// <summary>
	/// 重力フィールドの設定を編集
	/// </summary>
	/// <param name="selectedSystem">選択中のパーティクルシステム</param>
	void EditGravityFieldSettings(ParticleSystemSettings& selectedSystem);

	/// <summary>
	/// コンテナの中から要素を選択するためのComboBoxを作成する関数
	/// </summary>
	/// <typeparam name="Type">コンテナに格納されている値の型</typeparam>
	/// <param name="label">ラベル名</param>
	/// <param name="selectedName">選択された要素の名前</param>
	/// <param name="items">表示する名前とその設定が格納されたコンテナ</param>
	template<typename Type>
	void SelectFromMap(const char* label, std::string& selectedName, const std::map<std::string, Type>& items);

	/// <summary>
	/// 設定セクションを編集する関数
	/// </summary>
	/// <typeparam name="Type">設定項目のデータ型</typeparam>
	/// <param name="sectionName">編集対象のセクション名</param>
	/// <param name="settingsMap">設定名と設定データのマップ</param>
	/// <param name="currentEditSettingName">現在編集中の設定名</param>
	/// <param name="editCallback">設定項目に適用する編集コールバック関数</param>
	template<typename Type>
	void EditSettingsSection(const std::string& sectionName, std::map<std::string, Type>& settingsMap, std::string& currentEditSettingName, std::function<void(Type&)> editCallback);

	/// <summary>
	/// 指定された設定項目を削除する関数
	/// </summary>
	/// <typeparam name="Type">設定項目のデータ型</typeparam>
	/// <param name="settingsMap">設定名と設定データのマップ</param>
	/// <param name="selectedSettingName">削除対象の設定名</param>
	template<typename Type>
	void RemoveSetting(std::map<std::string, Type>& settingsMap, std::string& selectedSettingName);

private:
	//パーティクルマネージャー
	ParticleManager* particleManager_ = nullptr;

	//パーティクルシステム
	std::map<std::string, ParticleSystem*> particleSystems_{};

	//パーティクルエフェクトの設定
	std::map<std::string, ParticleEffectConfig> particleEffectConfigs_;
};


template<typename Type>
inline void ParticleEffectEditor::SelectFromMap(const char* label, std::string& selectedName, const std::map<std::string, Type>& items)
{
	//ラベルを表示
	ImGui::Text(label);

	//ComboBoxの作成
	if (ImGui::BeginCombo(label, selectedName.c_str()))
	{
		//全てのアイテムのキーをリスト表示
		for (const auto& [name, setting] : items)
		{
			//選択されたかどうかを判別
			const bool isSelected = (selectedName == name);

			//選択されていた場合、名前を更新
			if (ImGui::Selectable(name.c_str(), isSelected))
			{
				selectedName = name;
			}

			//選択されていた場合はデフォルトフォーカスを設定
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}

		//ComboBoxの終了
		ImGui::EndCombo();
	}
};

template<typename Type>
inline void ParticleEffectEditor::EditSettingsSection(const std::string& sectionName, std::map<std::string, Type>& settingsMap, std::string& currentEditSettingName, std::function<void(Type&)> editCallback)
{
	//設定を選択
	SelectFromMap(sectionName.c_str(), currentEditSettingName, settingsMap);

	//設定が見つからなければ処理を飛ばす
	auto settingIt = settingsMap.find(currentEditSettingName);
	if (settingIt == settingsMap.end()) return;

	//設定編集コールバックを実行
	editCallback(settingIt->second);

	//設定を削除
	std::string removeButtonText = sectionName + "を削除";
	ImGui::Spacing();
	if (ImGui::Button(removeButtonText.c_str()))
	{
		RemoveSetting(settingsMap, currentEditSettingName);
	}
}

template<typename Type>
inline void ParticleEffectEditor::RemoveSetting(std::map<std::string, Type>& settingsMap, std::string& selectedSettingName)
{
	settingsMap.erase(selectedSettingName);
	selectedSettingName.clear();
};