/**
 * @file LevelSelector.h
 * @brief ボスのレベルを管理するファイル
 * @author 青木智滉
 * @date
 */

#pragma once
#include "Engine/Components/Input/Input.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/UI/UIManager.h"

class LevelSelector
{
public:
	//難易度調整用のUIの列挙体
	enum class DifficultyUI
	{
		Easy,
		Normal,
		Hard,
		Menu,
		Count,
	};

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="enemy">敵</param>
	/// <param name="uiManager">UIマネージャー</param>
	void Initialize(Enemy* enemy, UIManager* uiManager);

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// レベル選択画面を表示フラグを設定
	/// </summary>
	void SetDifficultyMenuVisibility(bool isVisible);

	//難易度選択画面を開いているかどうかを取得
	const bool GetIsDifficultyMenuVisible() const { return isDifficultyMenuVisible_; };

private:
	/// <summary>
	/// 難易度UIを初期化
	/// </summary>
	void InitializeDifficultyUI();

	/// <summary>
	/// 難易度切り替えタイマーを更新
	/// </summary>
	void UpdateDifficultySwitchTimer();

	/// <summary>
	/// 難易度の選択処理
	/// </summary>
	void HandleDifficultySelection();

	/// <summary>
	/// 難易度の更新
	/// </summary>
	/// <param name="newDifficulty">新しい難易度</param>
	void UpdateDifficulty(const Enemy::Level newDifficulty);

	/// <summary>
	/// 難易度切り替え関連の状態をリセット
	/// </summary>
	void ResetSwitchParameters();

	/// <summary>
	/// 難易度調整用のUIを文字列に変換
	/// </summary>
	/// <param name="ui">何度調整用のUI</param>
	/// <returns>変換した文字列</returns>
	std::string DifficultyUIToString(const DifficultyUI ui);

private:
	//インプット
	Input* input_ = nullptr;

	//敵
	Enemy* enemy_ = nullptr;

	//UIマネージャー
	UIManager* uiManager_ = nullptr;

	//難易度選択用のUI
	std::vector<UIElement*> difficultyUI_{};

	//難易度選択画面を開いているかどうか
	bool isDifficultyMenuVisible_ = false;

	//切り替え可能かどうか
	bool isSwitchAllowed_ = false;

	//難易度切り替え用のタイマー
	float difficultySwitchTimer_ = 0.0f;
};

