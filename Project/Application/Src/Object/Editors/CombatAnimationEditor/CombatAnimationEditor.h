#pragma once
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Application/Src/Object/Editors/ParticleEffectEditor/ParticleEffectEditor.h"
#include "Application/Src/Object/Editors/CameraAnimationEditor/CameraAnimationEditor.h"
#include "Application/Src/Object/Editors/CombatAnimationEditor/AnimationEvents.h"
#include <Engine/Externals/nlohmann/json.hpp>
#include <fstream>
#include <map>

//前方宣言
class BaseCharacter;

//アニメーション速度の設定
struct AnimationSpeedConfig
{
	float duration;       //持続時間
	float animationSpeed; //アニメーションの速度
};

//アニメーションコントローラー
struct AnimationController
{
	std::vector<AnimationSpeedConfig> animationSpeedConfigs{};             //アニメーションの速度と持続時間
	std::vector<std::shared_ptr<AnimationEvent>> animationEvents{};        //アニメーションイベント
	Vector3 inPlaceAxis = { 1.0f,1.0f,1.0f };                              //アニメーションの動かす軸
	const int32_t GetAnimationEventCount(const EventType eventType) const; //アニメーションイベントの数を取得
};

/// <summary>
/// 戦闘のアニメーションを管理するクラス
/// </summary>
class CombatAnimationEditor
{
public:
	//キャラクターのアニメーションデータ
	struct CharacterAnimationData
	{
		std::map<std::string, AnimationController> animationControllers{}; //アニメーション名とコントローラーのマップ
		std::string selectedAnimation{}; //選択中のアニメーション名
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
	/// 編集するキャラクターを追加
	/// </summary>
	/// <param name="character">キャラクター</param>
	void AddEditableCharacter(BaseCharacter* character);

	/// <summary>
	/// アニメーションコントローラーを取得
	/// </summary>
	/// <param name="characterName">キャラクターの名前</param>
	/// <param name="animationName">アニメーションの名前</param>
	/// <returns>アニメーションコントローラー</returns>
	const AnimationController& GetAnimationController(const std::string& characterName, const std::string& animationName) const;

	//パーティクルエフェクトエディターを設定
	void SetParticleEffectEditor(const ParticleEffectEditor* particleEffectEditor) { particleEffectEditor_ = particleEffectEditor; };

	//カメラアニメーションエディターを設定
	void SetCameraAnimationEditor(const CameraAnimationEditor* cameraAnimationEditor) { cameraAnimationEditor_ = cameraAnimationEditor; };

private:
	//保存先ファイルパス
	const std::string kDirectoryPath = "Application/Resources/Config/CombatAnimations/";

	/// <summary>
	/// キャラクターのアニメーションを編集
	/// </summary>
	/// <param name="character">キャラクター</param>
	void EditCharacterAnimations(BaseCharacter* character);

	/// <summary>
	/// デバッグモードを切り替え
	/// </summary>
	/// <param name="character">キャラクター</param>
	/// <param name="characterAnimationData">キャラクターのアニメーションデータ</param>
	/// <param name="isDebug">デバッグのフラグ</param>
	void ToggleDebugMode(BaseCharacter* character, CharacterAnimationData& characterAnimationData, bool isDebug);

	/// <summary>
	/// 選択中のアニメーションの時間を編集
	/// </summary>
	/// <param name="animator">アニメーター</param>
	/// <param name="selectedAnimationName">選択中のアニメーションの名前</param>
	/// <param name="isDebug">デバッグ中かどうか</param>
	void EditAnimationTime(AnimatorComponent* animator, const std::string& selectedAnimationName, const bool isDebug);

	/// <summary>
	/// アニメーションコントローラーのコントロールを表示
	/// </summary>
	/// <param name="characterName">キャラクターの名前</param>
	/// <param name="animationControllerName">アニメーションコントローラーの名前</param>
	void DisplayAnimationControllerControls(const std::string& characterName, const std::string& animationControllerName);

	/// <summary>
	/// ファイルに保存
	/// </summary>
	/// <param name="characterName">キャラクターの名前</param>
	/// <param name="animationControllerName">アニメーションコントローラーの名前</param>
	void SaveFile(const std::string& characterName, const std::string& animationControllerName);

	/// <summary>
	/// アニメーション速度の設定をファイルに保存
	/// </summary>
	/// <param name="animationSpeedConfigs">アニメーションの速度設定</param>
	/// <param name="animationSpeedConfigsJson">アニメーションの速度設定のjsonオブジェクト</param>
	void SaveAnimationSpeedConfigs(const std::vector<AnimationSpeedConfig>& animationSpeedConfigs, nlohmann::json& animationSpeedConfigsJson);

	/// <summary>
	/// アニメーションイベントをファイルに保存
	/// </summary>
	/// <param name="animationEvents">アニメーションイベント</param>
	/// <param name="animationEventsJson">アニメーションイベントのjsonオブジェクト</param>
	void SaveAnimationEvents(const std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& animationEventsJson);

	/// <summary>
	/// 移動イベントを保存
	/// </summary>
	/// <param name="movementEvent">移動イベント</param>
	/// <param name="eventJson">イベントのjsonObject</param>
	void SaveMovementEvent(const MovementEvent* movementEvent, nlohmann::json& eventJson);

	/// <summary>
	/// 速度移動イベントを保存
	/// </summary>
	/// <param name="velocityMovementEvent">速度イベント</param>
	/// <param name="eventJson">イベントのjsonObject</param>
	void SaveVelocityMovementEvent(const VelocityMovementEvent* velocityMovementEvent, nlohmann::json& eventJson);

	/// <summary>
	/// イージング移動イベントを保存
	/// </summary>
	/// <param name="easingMovementEvent">イージング移動イベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveEasingMovementEvent(const EasingMovementEvent* easingMovementEvent, nlohmann::json& eventJson);

	/// <summary>
	/// 回転イベントを保存
	/// </summary>
	/// <param name="rotationEvent">回転イベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveRotationEvent(const RotationEvent* rotationEvent, nlohmann::json& eventJson);

	/// <summary>
	/// 攻撃ベントを保存
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveAttackEvent(const AttackEvent* attackEvent, nlohmann::json& eventJson);

	/// <summary>
	/// エフェクトイベントを保存
	/// </summary>
	/// <param name="effectEvent">エフェクトイベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveEffectEvent(const EffectEvent* effectEvent, nlohmann::json& eventJson);

	/// <summary>
	/// カメラアニメーションイベントを保存
	/// </summary>
	/// <param name="cameraAnimationEvent">カメラアニメーションイベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveCameraAnimationEvent(const CameraAnimationEvent* cameraAnimationEvent, nlohmann::json& eventJson);

	/// <summary>
	/// キャンセルイベントを保存
	/// </summary>
	/// <param name="cancelEvent">キャンセルイベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveCancelEvent(const CancelEvent* cancelEvent, nlohmann::json& eventJson);

	/// <summary>
	/// 先行入力イベントを保存
	/// </summary>
	/// <param name="bufferedActionEvent">先行入力イベント</param>
	/// <param name="eventJson">イベントのjsonobject</param>
	void SaveBufferedActionEvent(const BufferedActionEvent* bufferedActionEvent, nlohmann::json& eventJson);

	/// <summary>
	/// 全てのファイルを読み込む
	/// </summary>
	void LoadFiles();

	/// <summary>
	/// ファイルを読み込む
	/// </summary>
	/// <param name="characterName">キャラクターの名前</param>
	/// <param name="animationControllerName">アニメーションコントローラーの名前</param>
	void LoadFile(const std::string& characterName, const std::string& animationControllerName);

	/// <summary>
	/// アニメーション速度設定を読み込む
	/// </summary>
	/// <param name="animationSpeedConfigs">アニメーションの速度設定</param>
	/// <param name="animationSpeedConfigsJson">アニメーションの速度設定のjsonオブジェクト</param>
	void LoadAnimationSpeedConfigs(std::vector<AnimationSpeedConfig>& animationSpeedConfigs, nlohmann::json& animationSpeedConfigsJson);

	/// <summary>
	/// アニメーションイベントを読み込む
	/// </summary>
	/// <param name="animationEvents">アニメーションイベント</param>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	void LoadAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, nlohmann::json& eventJson);

	/// <summary>
	/// 移動イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>移動イベント</returns>
	std::shared_ptr<MovementEvent> LoadMovementEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// 移動イベントを初期化
	/// </summary>
	/// <param name="movementEvent">移動イベント</param>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	void InitializeCommonMovementEvent(const std::shared_ptr<MovementEvent>& movementEvent, const nlohmann::json& eventJson) const;

	/// <summary>
	/// 速度移動イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>速度移動イベント</returns>
	std::shared_ptr<VelocityMovementEvent> LoadVelocityMovementEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// イージング移動イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>イージング移動イベント</returns>
	std::shared_ptr<EasingMovementEvent> LoadEasingMovementEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// 回転イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>回転イベント</returns>
	std::shared_ptr<RotationEvent> LoadRotationEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// 攻撃イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>攻撃イベント</returns>
	std::shared_ptr<AttackEvent> LoadAttackEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// エフェクトイベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>エフェクトイベント</returns>
	std::shared_ptr<EffectEvent> LoadEffectEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// カメラアニメーションイベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>カメラアニメーションイベント</returns>
	std::shared_ptr<CameraAnimationEvent> LoadCameraAnimationEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// キャンセルイベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>キャンセルイベント</returns>
	std::shared_ptr<CancelEvent> LoadCancelEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// 先行入力イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>先行入力イベント</returns>
	std::shared_ptr<BufferedActionEvent> LoadBufferedActionEvent(const nlohmann::json& eventJson) const;

	/// <summary>
	/// アニメーション速度の設定を管理
	/// </summary>
	/// <param name="animationSpeedConfigs">アニメーション速度の設定</param>
	void ManageAnimationSpeedConfigs(std::vector<AnimationSpeedConfig>& animationSpeedConfigs);

	/// <summary>
	/// アニメーションの速度の設定を追加
	/// </summary>
	/// <param name="animationSpeedConfigs">アニメーションの速度の設定</param>
	void AddAnimationSpeedConfig(std::vector<AnimationSpeedConfig>& animationSpeedConfigs);

	/// <summary>
	/// 全てのアニメーション速度の設定を編集
	/// </summary>
	/// <param name="animationSpeedConfigs">アニメーションの速度の設定</param>
	void EditAnimationSpeedConfigs(std::vector<AnimationSpeedConfig>& animationSpeedConfigs);

	/// <summary>
	/// アニメーションイベントを管理
	/// </summary>
	/// <param name="animationEvents">全てのアニメーションイベント</param>
	void ManageAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// アニメーションイベントを追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddAnimationEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 移動イベントを追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddMovementEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 回転イベントの追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddRotationEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 攻撃イベントを追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddAttackEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// エフェクトイベントを追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddEffectEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// カメラアニメーションイベントを追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddCameraAnimationEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// キャンセルイベントを追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddCancelEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 先行入力イベントを追加
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void AddBufferedActionEvent(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// アニメーションイベントを編集
	/// </summary>
	/// <param name="animationEvents">アニメーションイベントの配列</param>
	void EditAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents);

	/// <summary>
	/// 移動イベントを編集
	/// </summary>
	/// <param name="velocityMovementEvent">移動イベント</param>
	void EditMovementEvent(MovementEvent* movementEvent);

	/// <summary>
	/// 回転イベントの編集
	/// </summary>
	/// <param name="rotationEvent">回転イベント</param>
	void EditRotationEvent(RotationEvent* rotationEvent);

	/// <summary>
	/// 攻撃イベントを編集
	/// </summary>
	/// <param name="attackEvent">攻撃イベント</param>
	void EditAttackEvent(AttackEvent* attackEvent);

	/// <summary>
	/// エフェクトイベントを編集
	/// </summary>
	/// <param name="effectEvent">エフェクトイベント</param>
	void EditEffectEvent(EffectEvent* effectEvent);

	/// <summary>
	/// カメラアニメーションイベントの編集
	/// </summary>
	/// <param name="cameraAnimationEvent">カメラアニメーションイベント</param>
	void EditCameraAnimationEvent(CameraAnimationEvent* cameraAnimationEvent);

	/// <summary>
	/// キャンセルイベントを編集
	/// </summary>
	/// <param name="cancelEvent">キャンセルイベント</param>
	void EditCancelEvent(CancelEvent* cancelEvent);

	/// <summary>
	/// 先行入力イベントを編集
	/// </summary>
	/// <param name="bufferedActionEvent">先行入力イベント</param>
	void EditBufferedActionEvent(BufferedActionEvent* bufferedActionEvent);

	/// <summary>
	/// コンテナの中から要素を選択するためのComboBoxを作成する関数
	/// </summary>
	/// <typeparam name="Type">コンテナに格納されている値の型</typeparam>
	/// <param name="label">ラベル名</param>
	/// <param name="selectedName">選択された要素の名前</param>
	/// <param name="items">表示する名前とその設定が格納されたコンテナ</param>
	/// <param name="showLabel">ラベルを表示するかどうか</param>
	template<typename Type>
	void SelectFromMap(const char* label, std::string& selectedName, const std::map<std::string, Type>& items, const bool showLabel);

	/// <summary>
	/// 選択された要素の名前を更新するComboBoxを作成
	/// </summary>
	/// <typeparam name="Type">コンテナに格納されている値の型</typeparam>
	/// <param name="label">ラベル名</param>
	/// <param name="selectedName">選択された要素の名前</param>
	/// <param name="items">表示する選択肢を格納した配列</param>
	/// <param name="itemCount">選択肢の数</param>
	template<typename Type>
	void EditCombo(const char* label, std::string& selectedName, const Type* items, int itemCount);

	/// <summary>
	/// アニメーションイベントの時間を編集
	/// </summary>
	/// <typeparam name="Type">アニメーションイベントの型</typeparam>
	/// <param name="animationEvent">アニメーションイベント</param>	
	template<typename Type>
	void EditEventTime(Type* animationEvent);

private:
	//アニメーションイベント
	std::map<std::string, CharacterAnimationData> characterAnimations_{};

	//編集するキャラクター
	std::map<std::string, BaseCharacter*> editableCharacters_{};

	//パーティクルエフェクトエディター
	const ParticleEffectEditor* particleEffectEditor_ = nullptr;

	//カメラアニメーションエディター
	const CameraAnimationEditor* cameraAnimationEditor_ = nullptr;
};


template<typename Type>
inline void CombatAnimationEditor::SelectFromMap(const char* label, std::string& selectedName, const std::map<std::string, Type>& items, const bool showLabel)
{
	//ラベルを表示
	if (showLabel)
	{
		ImGui::Text(label);
	}

	//コンボボックスの作成
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

		//コンボボックスの終了
		ImGui::EndCombo();
	}
}

template<typename Type>
inline void CombatAnimationEditor::EditCombo(const char* label, std::string& selectedName, const Type* items, int itemCount)
{
	if (ImGui::BeginCombo(label, selectedName.c_str()))
	{
		for (int i = 0; i < itemCount; ++i)
		{
			//選択されたかどうかのフラグ
			bool isSelected = (selectedName == items[i]);
			//項目を追加
			if (ImGui::Selectable(items[i], isSelected))
			{
				//選択された値を更新
				selectedName = items[i];
			}

			//初めて選択された項目にチェックマークを表示
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

template<typename Type>
inline void CombatAnimationEditor::EditEventTime(Type* animationEvent)
{
	ImGui::DragFloat("Start Event Time", &animationEvent->startEventTime, 0.001f);
	ImGui::DragFloat("End Event Time", &animationEvent->endEventTime, 0.001f);
}