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

	//キャラクターデータ
	struct CharacterData
	{
		BaseCharacter* character;                                         //編集対象キャラクター
		CharacterAnimationData animationData;                             //アニメーションデータ
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
	std::shared_ptr<MovementEvent> LoadMovementEvent(const nlohmann::json& eventJson);

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
	std::shared_ptr<VelocityMovementEvent> LoadVelocityMovementEvent(const nlohmann::json& eventJson);

	/// <summary>
	/// イージング移動イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>イージング移動イベント</returns>
	std::shared_ptr<EasingMovementEvent> LoadEasingMovementEvent(const nlohmann::json& eventJson);

	/// <summary>
	/// 回転イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>回転イベント</returns>
	std::shared_ptr<RotationEvent> LoadRotationEvent(const nlohmann::json& eventJson);

	/// <summary>
	/// 攻撃イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>攻撃イベント</returns>
	std::shared_ptr<AttackEvent> LoadAttackEvent(const nlohmann::json& eventJson);

	/// <summary>
	/// エフェクトイベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>エフェクトイベント</returns>
	std::shared_ptr<EffectEvent> LoadEffectEvent(const nlohmann::json& eventJson);

	/// <summary>
	/// カメラアニメーションイベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>カメラアニメーションイベント</returns>
	std::shared_ptr<CameraAnimationEvent> LoadCameraAnimationEvent(const nlohmann::json& eventJson);

	/// <summary>
	/// キャンセルイベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>キャンセルイベント</returns>
	std::shared_ptr<CancelEvent> LoadCancelEvent(const nlohmann::json& eventJson);

	/// <summary>
	/// 先行入力イベントを生成して返す
	/// </summary>
	/// <param name="eventJson">アニメーションイベントのjsonオブジェクト</param>
	/// <returns>先行入力イベント</returns>
	std::shared_ptr<BufferedActionEvent> LoadBufferedActionEvent(const nlohmann::json& eventJson);

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
	/// <param name="character">キャラクター</param>
	void ManageAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, const BaseCharacter* character);

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
	/// <param name="character">キャラクター</param>
	void EditAnimationEvents(std::vector<std::shared_ptr<AnimationEvent>>& animationEvents, const BaseCharacter* character);

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
	/// <param name="soundEffects">サウンドエフェクトの名前一覧</param>
	void EditEffectEvent(EffectEvent* effectEvent, const std::vector<std::string>& soundEffects);

	/// <summary>
	/// カメラアニメーションイベントの編集
	/// </summary>
	/// <param name="cameraAnimationEvent">カメラアニメーションイベント</param>
	void EditCameraAnimationEvent(CameraAnimationEvent* cameraAnimationEvent);

	/// <summary>
	/// キャンセルイベントを編集
	/// </summary>
	/// <param name="cancelEvent">キャンセルイベント</param>
	/// <param name="actions">アクションの名前一覧</param>
	void EditCancelEvent(CancelEvent* cancelEvent, const std::vector<std::string>& actions);

	/// <summary>
	/// 先行入力イベントを編集
	/// </summary>
	/// <param name="bufferedActionEvent">先行入力イベント</param>
	/// <param name="actions">アクションの名前一覧</param>
	void EditBufferedActionEvent(BufferedActionEvent* bufferedActionEvent, const std::vector<std::string>& actions);

	/// <summary>
	/// 選択された要素の名前を更新するComboBoxを作成
	/// </summary>
	/// <param name="label">ラベル名</param>
	/// <param name="selectedName">選択された要素の名前</param>
	/// <param name="items">表示する選択肢を格納した配列</param>
	void EditCombo(const char* label, std::string& selectedName, const std::vector<std::string>& items);

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
	/// 列挙型を選択するためのコンボボックスを作成する関数
	/// </summary>
	/// <typeparam name="Type">列挙型の型</typeparam>
	/// <param name="label">ラベル名</param>
	/// <param name="enumValue">選択された列挙型の名前</param>
	/// <param name="items">列挙型の文字列配列</param>
	/// <param name="itemCount">列挙型の文字列配列のサイズ</param>
	template<typename Type>
	void SelectFromEnum(const char* label, Type& enumValue, const char* items[], const int32_t itemCount);

	/// <summary>
	/// jsonの値と指定されたタイプの配列を比較し、対応するインデックスを基に値を設定する関数
	/// </summary>
	/// <typeparam name="Type">設定する列挙型の型</typeparam>
	/// <param name="jsonValue">jsonの値</param>
	/// <param name="target">設定先の列挙型変数</param>
	/// <param name="typesArray">列挙型の文字列配列</param>
	/// <param name="arraySize">文字列配列のサイズ</param>
	template<typename Type>
	void SetEnumFromJson(const std::string& jsonValue, Type& target, const char* typesArray[], const int32_t arraySize);

	/// <summary>
	/// アニメーションイベントの時間を編集
	/// </summary>
	/// <typeparam name="Type">アニメーションイベントの型</typeparam>
	/// <param name="animationEvent">アニメーションイベント</param>	
	template<typename Type>
	void EditEventTime(Type* animationEvent);

private:
	//キャラクターデータ
	std::map<std::string, CharacterData> characterDatas_{};

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
inline void CombatAnimationEditor::SelectFromEnum(const char* label, Type& enumValue, const char* items[], const int32_t itemCount)
{
	//選択中の列挙体のインデックス
	int selectedIndex = static_cast<int>(enumValue);
	//列挙体を選択
	if (ImGui::Combo(label, &selectedIndex, items, itemCount))
	{
		enumValue = static_cast<Type>(selectedIndex);
	}
}

template<typename Type>
inline void CombatAnimationEditor::SetEnumFromJson(const std::string& jsonValue, Type& target, const char* typesArray[], const int32_t arraySize)
{
	//配列の各要素を走査
	for (int32_t i = 0; i < arraySize; ++i)
	{
		//JSONの値が配列内の文字列と一致する場合
		if (jsonValue == typesArray[i])
		{
			//一致したインデックスを列挙型の値として設定
			target = static_cast<Type>(i);
			return;
		}
	}
}

template<typename Type>
inline void CombatAnimationEditor::EditEventTime(Type* animationEvent)
{
	ImGui::DragFloat("開始時間", &animationEvent->startEventTime, 0.001f);
	ImGui::DragFloat("終了時間", &animationEvent->endEventTime, 0.001f);
}