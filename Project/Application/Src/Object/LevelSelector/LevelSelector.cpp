/**
 * @file LevelSelector.cpp
 * @brief ボスのレベルを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "LevelSelector.h"

void LevelSelector::Initialize(Enemy* enemy, UIManager* uiManager)
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//敵を設定
	enemy_ = enemy;

	//UIマネージャーを設定
	uiManager_ = uiManager;

	//難易度選択用のUIを初期化
	InitializeDifficultyUI();

	//難易度の更新
	UpdateDifficulty(enemy_->GetLevel());
}

void LevelSelector::Update()
{
	//難易度切り替え用のタイマーの更新
	UpdateDifficultySwitchTimer();

	//難易度を選択
	HandleDifficultySelection();
}

void LevelSelector::SetDifficultyMenuVisibility(bool isVisible)
{
	//難易度選択画面の表示フラグを設定
	isDifficultyMenuVisible_ = isVisible;

	//全てのUIの描画フラグを設定
	for (UIElement* difficultyUI : difficultyUI_)
	{
		difficultyUI->SetIsVisible(isVisible);
	}
}

void LevelSelector::InitializeDifficultyUI()
{
	for (int32_t i = 0; i < static_cast<int32_t>(DifficultyUI::Count); ++i)
	{
		//難易度に対応するUIを取得
		UIElement* ui = uiManager_->GetUI<UIElement>(DifficultyUIToString(static_cast<DifficultyUI>(i)));
		//初期状態では非表示
		ui->SetIsVisible(false);
		//配列に追加
		difficultyUI_.push_back(ui); 
	}
}

void LevelSelector::UpdateDifficultySwitchTimer()
{
	//切り替え用のタイマーが切り替え時間を超えていたら変更可能にする
	if (difficultySwitchTimer_ > 0.0f)
	{
		//難易度切り替えのタイマーを進める
		difficultySwitchTimer_ -= GameTimer::GetDeltaTime();
	}
	else
	{
		//切り替え時間を超えた場合切り替えを許可
		isSwitchAllowed_ = true;
	}
}

void LevelSelector::HandleDifficultySelection()
{
	//レベル選択画面を開いていない場合は処理を飛ばす
	if (!isDifficultyMenuVisible_ || !isSwitchAllowed_) return;

	//スティックの入力の閾値
	static const float kInputThreshold = 0.7f;

	//スティックの入力値を取得
	Vector3 stickInput = {
		input_->GetLeftStickX(),
		input_->GetLeftStickY(),
		0.0f
	};

	//入力が閾値を超えていない場合は終了
	if (Mathf::Length(stickInput) <= kInputThreshold) return;

	//現在の敵の難易度を取得
	Enemy::Level currentDifficulty = enemy_->GetLevel();

	//上方向入力の場合は難易度を下げる
	if (stickInput.y > 0 && currentDifficulty > Enemy::Level::Easy)
	{
		currentDifficulty = static_cast<Enemy::Level>(static_cast<int>(currentDifficulty) - 1);
	}
	//下方向入力の場合は難易度を上げる
	else if (stickInput.y < 0 && currentDifficulty < Enemy::Level::Count - 1)
	{
		currentDifficulty = static_cast<Enemy::Level>(static_cast<int>(currentDifficulty) + 1);
	}

	//難易度とUIの更新
	UpdateDifficulty(currentDifficulty);

	//難易度切り替え関連の状態をリセット
	ResetSwitchParameters();
}

void LevelSelector::UpdateDifficulty(const Enemy::Level newDifficulty)
{
	//UIの色を定義
	static const Vector4 kDefaultColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	static const Vector4 kSelectedColor = { 0.814f, 0.794f, 0.148f, 1.0f };

	//新しい難易度を敵に設定
	enemy_->SetLevel(newDifficulty);

	//UIの色を更新
	for (int32_t i = 0; i < Enemy::Level::Count; ++i)
	{
		Vector4 color = (newDifficulty == i) ? kSelectedColor : kDefaultColor;
		difficultyUI_[i]->GetSprite()->SetColor(color);
	}
}

void LevelSelector::ResetSwitchParameters()
{
	//切り替え時間
	static const float kSwitchInterval = 0.2f;

	//切り替え許可フラグとタイマーを初期化
	isSwitchAllowed_ = false;
	difficultySwitchTimer_ = kSwitchInterval;
}

std::string LevelSelector::DifficultyUIToString(const DifficultyUI ui)
{
	switch (ui)
	{
	case DifficultyUI::Easy: return "Easy";
	case DifficultyUI::Normal: return "Normal";
	case DifficultyUI::Hard: return "Hard";
	case DifficultyUI::Menu: return "DifficultyMenu";
	default: return "";
	}
}