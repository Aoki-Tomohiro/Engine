/**
 * @file Player.cpp
 * @brief プレイヤーを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"

void Player::Initialize()
{
	//基底クラスの初期化
	BaseCharacter::Initialize();

	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//スキルクール弾マネージャーの初期化
	InitializeSkillCooldownManager();

	//ダメージエフェクトのスプライトを生成
	InitializeDamageEffectSprite();

	//状態の初期化
	ChangeState("Move");
}

void Player::Update()
{
	//タイトルシーンにいる場合は基底クラスの更新だけする
	if (isInTitleScene_)
	{
		GameObject::Update();
		return;
	}

	//ボタンの入力状態の更新
	UpdateButtonStates();

	//スキルのクールダウンの更新
	UpdateSkillCooldowns();

	//ストンプのフラグの更新
	UpdateStompFlag();

	//魔法攻撃の更新
	UpdateMagicAttack();

	//ダメージエフェクトの更新
	UpdateDamageEffect();

	//基底クラスの更新
	BaseCharacter::Update();

	//QTEの要素の更新
	UpdateQTEElements();
}

void Player::DrawUI()
{
	//基底クラスの描画
	BaseCharacter::DrawUI();

	//ダメージエフェクトのスプライトを描画
	damageEffect_.sprite->Draw();
}

void Player::OnCollision(GameObject* gameObject)
{
	//衝突相手が敵の場合
	if (Enemy* enemy = dynamic_cast<Enemy*>(gameObject))
	{
		//プレイヤーと敵のAABBコライダーを取得
		AABBCollider* AABB1 = gameObject->GetComponent<AABBCollider>();

		//2つのAABBコライダー間のオーバーラップ軸を計算
		Vector3 overlapAxis = {
			std::min<float>(collider_->GetWorldCenter().x + collider_->GetMax().x, AABB1->GetWorldCenter().x + AABB1->GetMax().x) -
			std::max<float>(collider_->GetWorldCenter().x + collider_->GetMin().x, AABB1->GetWorldCenter().x + AABB1->GetMin().x),
			std::min<float>(collider_->GetWorldCenter().y + collider_->GetMax().y, AABB1->GetWorldCenter().y + AABB1->GetMax().y) -
			std::max<float>(collider_->GetWorldCenter().y + collider_->GetMin().y, AABB1->GetWorldCenter().y + AABB1->GetMin().y),
			std::min<float>(collider_->GetWorldCenter().z + collider_->GetMax().z, AABB1->GetWorldCenter().z + AABB1->GetMax().z) -
			std::max<float>(collider_->GetWorldCenter().z + collider_->GetMin().z, AABB1->GetWorldCenter().z + AABB1->GetMin().z),
		};

		//プレイヤーと敵のTransformComponentを取得
		TransformComponent* transform1 = GetComponent<TransformComponent>();
		TransformComponent* transform2 = gameObject->GetComponent<TransformComponent>();

		//最小のオーバーラップ軸に基づいて衝突方向を決定
		Vector3 directionAxis{};
		if (overlapAxis.x < overlapAxis.y && overlapAxis.x < overlapAxis.z) {
			//X軸方向で最小の重なりが発生している場合
			directionAxis.x = (transform1->worldTransform_.translation_.x < transform2->worldTransform_.translation_.x) ? -1.0f : 1.0f;
			directionAxis.y = 0.0f;
		}
		else if (overlapAxis.y < overlapAxis.x && overlapAxis.y < overlapAxis.z) {
			////Y軸方向で最小の重なりが発生している場合
			//directionAxis.y = (transform1->worldTransform_.translation_.y < transform2->worldTransform_.translation_.y) ? -1.0f : 1.0f;
			//directionAxis.x = 0.0f;
		}
		else if (overlapAxis.z < overlapAxis.x && overlapAxis.z < overlapAxis.y)
		{
			//Z軸方向で最小の重なりが発生している場合
			directionAxis.z = (transform1->worldTransform_.translation_.z < transform2->worldTransform_.translation_.z) ? -1.0f : 1.0f;
			directionAxis.x = 0.0f;
			directionAxis.y = 0.0f;
		}

		//衝突方向に基づいてプレイヤーの位置を修正
		transform1->worldTransform_.translation_ += overlapAxis * directionAxis;
	}
	else
	{
		//状態の衝突判定処理
		currentState_->OnCollision(gameObject);
	}
}

void Player::ApplyDamageAndKnockback(const KnockbackParameters& knockbackSettings, const float damage, const bool transitionToStun)
{
	//ダメージエフェクトのタイマーと色を設定
	damageEffect_.timer = 0.0f;
	damageEffect_.color.w = 0.2f;

	//スタン状態に遷移する場合はカメラアニメーションを止める
	if (transitionToStun)
	{
		cameraController_->StopAnimation();
	}

	//基底クラスの呼び出し
	BaseCharacter::ApplyDamageAndKnockback(knockbackSettings, damage, transitionToStun);
}

void Player::LookAtEnemy()
{
	//プレイヤーの腰のジョイントのワールド座標を取得
	Vector3 playerPosition = GetJointWorldPosition("mixamorig:Hips");

	//敵の座標を取得
	Vector3 enemyPosition = gameObjectManager_->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

	//敵の方向へのベクトルを計算
	Vector3 rotateVector = Mathf::Normalize(enemyPosition - playerPosition);

	//Y軸にだけ回転させたいのでY成分を0にする
	rotateVector.y = 0.0f;

	//回転処理
	Rotate(Mathf::Normalize(rotateVector));
}

void Player::InitializeActionMap()
{
	//定数を定義
	static const int kFirstSkillSetIndex = 0;
	static const int kSecondSkillSetIndex = 1;

	//アクションマップの初期化
	actionMap_ = {
		{"None", ActionCondition{[this]() {return true; }, [this]() {return true; }}},
		{"Move", ActionCondition{[this]() {return true; }, [this]() { return Mathf::Length({ input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() }) > rootParameters_.walkThreshold; }}},
		{"Jump", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::A].isTriggered && (GetPosition().y <= GetAdjustGroundLevel() || GetActionFlag(Player::ActionFlag::kCanStomp)); }}},
		{"Dodge", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::RB].isTriggered; }}},
		{"Dash", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::LB].isTriggered; }}},
		{"Attack", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::X].isTriggered; }}},
		{"Magic", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::LT].isTriggered && actionFlags_[Player::ActionFlag::kMagicAttackEnabled]; }}},
		{"ChargeMagic", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::LT].isTriggered && actionFlags_[Player::ActionFlag::kChargeMagicAttackEnabled]; }}},
		{"FallingAttack", ActionCondition{[this]() {return true; }, [this]() {return CheckFallingAttack(); }}},
		{"LaunchAttack", ActionCondition{[this]() {return IsAbilityUsable(skillPairSets_[kFirstSkillSetIndex].first); }, [this]() {return buttonStates_[Player::ButtonType::Y].isTriggered; }}},
		{"SpinAttack", ActionCondition{[this]() {return IsAbilityUsable(skillPairSets_[kFirstSkillSetIndex].second); }, [this]() {return buttonStates_[Player::ButtonType::B].isTriggered; }}},
	};

	//QTEのアクションの初期化
	for (const auto& [key, condition] : actionMap_)
	{
		//アクション名がNoneまたはMoveの場合は処理を飛ばす
		if (key == "None" || key == "Move") continue;

		//QTEのアクションを追加
		qteActions_.push_back(key);
	}
}

void Player::InitializeUISprites()
{
	////基底クラスの呼び出し
	BaseCharacter::InitializeUISprites();

	//スキルのUIを設定
	SetupSkillUIForPairs();

	//QTEのUIを取得
	SetupQTEUI();
}

void Player::SetupSkillUIForPairs()
{
	//UIマネージャーが設定されていなければ処理を飛ばす
	if (!uiManager_) return;

	//全てのスキルペアのUIを設定
	for (size_t i = 0; i < skillPairSets_.size(); ++i)
	{
		//スキルのペアを取得
		const auto& skillPair = skillPairSets_[i];

		//1番目のスキルUI設定
		SetupSkillUI(i, skillPair.first, 0);

		//2番目のスキルUI設定
		SetupSkillUI(i, skillPair.second, 1);
	}
}

void Player::SetupSkillUI(size_t index, const SkillParameters& skillParameters, size_t skillIndex)
{
	//スキルの名前に基づいてUIを取得
	DynamicUI* ui = uiManager_->GetUI<DynamicUI>(skillParameters.name + "BarUI");

	//自動でゲージを減らさないようにする
	ui->SetAutoDecrement(false);

	//残り時間をクールタイム時間に設定
	ui->SetTimeRemaining(skillParameters.cooldownDuration);

	//経過時間をリセット
	ui->SetElapsedTime(skillParameters.cooldownDuration);

	//スキルUIを更新
	if (skillIndex == 0)
	{
		skillUI_[index].first = ui;
	}
	else
	{
		skillUI_[index].second = ui;
	}
}

void Player::SetupQTEUI()
{
	//UIマネージャーが設定されていなければ処理を飛ばす
	if (!uiManager_) return;

	//全てのQTEのアクションのUIを追加
	for (const std::string& actionName : qteActions_)
	{
		//新しく追加するQTEのUI
		QTEUI ui{};

		//UIの名前
		std::string uiName = "QTE" + actionName;

		//ボタンのUIを追加
		UIElement* buttonUI = uiManager_->GetUI<UIElement>(uiName + "ButtonUI");
		ui.buttonUI = { buttonUI, buttonUI->GetPosition() };

		//アクション名のUIを追加
		UIElement* actionUI = uiManager_->GetUI<UIElement>(uiName + "ActionUI");
		ui.actionUI = { actionUI, actionUI->GetPosition() };

		//受付時間のUIを追加
		DynamicUI* barUI = uiManager_->GetUI<DynamicUI>(uiName + "BarUI");
		ui.barUI = { barUI, barUI->GetPosition() };

		//QTEUIのマップに追加
		qteUIMap_[actionName] = ui;
	}
}

void Player::InitializeSkillCooldownManager()
{
	//スキルクールダウンマネージャーの初期化
	skillCooldownManager_ = std::make_unique<SkillCooldownManager>();
	//スキルを追加
	for (const auto skillPairSet : skillPairSets_)
	{
		skillCooldownManager_->AddSkill(skillPairSet.first.name, skillPairSet.first.cooldownDuration);
		skillCooldownManager_->AddSkill(skillPairSet.second.name, skillPairSet.second.cooldownDuration);
	}
}

void Player::InitializeDamageEffectSprite()
{
	//ダメージエフェクト用のスプライトの生成
	damageEffect_.sprite.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	damageEffect_.sprite->SetColor(damageEffect_.color);
	damageEffect_.sprite->SetTextureSize({ 1280.0f,720.0f });
}

void Player::UpdateButtonStates()
{
	//全てのボタンの状態を更新
	for (int32_t i = 0; i < kMaxButtons; ++i)
	{
		UpdateButtonState(i);
	}
}

void Player::UpdateButtonState(const int32_t buttonIndex)
{
	//ボタンの押下状態を判定
	bool isPressButtonEnter = CheckButtonPress(buttonIndex);

	//ボタンが押された瞬間の判定
	if (isPressButtonEnter)
	{
		//押されたフラグを立てる
		buttonStates_[buttonIndex].isPressed = true;
		//押し始めた時間をリセット
		buttonStates_[buttonIndex].pressedFrame = 0;
	}

	//ボタンが押されている場合の処理
	if (buttonStates_[buttonIndex].isPressed)
	{
		//押されたフレーム数を増やす
		buttonStates_[buttonIndex].pressedFrame++;

		//同時押しに対応するために少し余裕を持たせる
		const int32_t kPressFrameThreshold = 3;
		if (buttonStates_[buttonIndex].pressedFrame >= kPressFrameThreshold)
		{
			//トリガーフラグを立てる
			buttonStates_[buttonIndex].isTriggered = true;
			//押された状態をリセット
			buttonStates_[buttonIndex].isPressed = false;
		}
	}
	else
	{
		//トリガーフラグをリセット
		buttonStates_[buttonIndex].isTriggered = false;
	}

	//ボタンの離脱状態を判定
	bool isPressButtonExit = CheckButtonRelease(buttonIndex);

	//ボタンが離された瞬間の判定
	if (isPressButtonExit)
	{
		//ボタンが押されているときに離されたらトリガーフラグを立てる
		if (buttonStates_[buttonIndex].isPressed)
		{
			buttonStates_[buttonIndex].isTriggered = true;
		}
		//離されたフラグを立てる
		buttonStates_[buttonIndex].isReleased = true;
		//押された状態をリセット
		buttonStates_[buttonIndex].isPressed = false;
		//押し始めた時間をリセット
		buttonStates_[buttonIndex].pressedFrame = 0;
	}
	else
	{
		//離されたフラグをリセット
		buttonStates_[buttonIndex].isReleased = false;
	}
}

bool Player::CheckButtonPress(const int32_t buttonIndex) const
{
	//ボタンの押下判定を返す
	if (buttonIndex == ButtonType::LT)
	{
		return input_->GetLeftTriggerValue() > kTriggerThreshold;
	}
	else if (buttonIndex == ButtonType::XA)
	{
		return input_->IsPressButton(XINPUT_GAMEPAD_X) && input_->IsPressButton(XINPUT_GAMEPAD_A);
	}
	return input_->IsPressButtonEnter(buttonMappings_[buttonIndex]);
}

bool Player::CheckButtonRelease(const int32_t buttonIndex) const
{
	//ボタンの離脱判定を返す
	if (buttonIndex == ButtonType::LT)
	{
		return input_->GetLeftTriggerValue() <= kTriggerThreshold;
	}
	else if (buttonIndex == ButtonType::XA)
	{
		return !input_->IsPressButton(XINPUT_GAMEPAD_X) && !input_->IsPressButton(XINPUT_GAMEPAD_A);
	}
	return input_->IsPressButtonExit(buttonMappings_[buttonIndex]);
}

void Player::UpdateCooldownTimer()
{
	//クールダウンのタイマーを進める
	if (magicAttackWork_.cooldownTimer > 0.0f)
	{
		magicAttackWork_.cooldownTimer -= GameTimer::GetDeltaTime() * timeScale_;
	}
}

void Player::UpdateStompFlag()
{
	//プレイヤーの指定したジョイントのワールド座標を取得
	Vector3 playerPosition = GetJointWorldPosition("mixamorig:Hips");

	//敵の指定したジョイントのワールド座標を取得
	Vector3 enemyPosition = gameObjectManager_->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

	//距離を計算しストンプ可能な距離内かを判定
	bool canStomp = Mathf::Length(enemyPosition - playerPosition) < kStompRange_;
	SetActionFlag(ActionFlag::kCanStomp, canStomp);
}

void Player::UpdateMagicAttack()
{
	//クールダウンのタイマーを進める
	UpdateCooldownTimer();

	//Yボタン押している場合
	if (input_->GetLeftTriggerValue() > kTriggerThreshold)
	{
		HandleMagicCharge();
	}
	else
	{
		//魔法をチャージ中の場合、フラグをリセット
		if (magicAttackWork_.isCharging)
		{
			magicAttackWork_.isCharging = false;
			magicAttackWork_.chargeTimer = 0.0f;
		}
	}
}

void Player::HandleMagicCharge()
{
	//魔法がチャージされていない場合はチャージを開始
	if (!magicAttackWork_.isCharging)
	{
		magicAttackWork_.isCharging = true;
		magicAttackWork_.chargeTimer = 0.0f;
	}
	//チャージ中の場合はチャージを継続
	else
	{
		ContinueCharging();
	}
}

void Player::ContinueCharging()
{
	//チャージタイマーを進める
	magicAttackWork_.chargeTimer += GameTimer::GetDeltaTime() * timeScale_;

	//チャージの閾値をチェック
	if (magicAttackWork_.chargeTimer > magicAttackParameters_.chargeTimeThreshold)
	{
		//チャージフラグを立てる
		actionFlags_[ActionFlag::kChargeMagicAttackEnabled] = true;
		actionFlags_[ActionFlag::kMagicAttackEnabled] = false;
	}
	else
	{
		//魔法攻撃のクールタイムを確認
		if (magicAttackWork_.cooldownTimer <= 0.0f)
		{
			//魔法攻撃のフラグを立てる
			actionFlags_[ActionFlag::kMagicAttackEnabled] = true;
			actionFlags_[ActionFlag::kChargeMagicAttackEnabled] = false;
		}
	}
}

void Player::UpdateSkillCooldowns()
{
	//スキルクールダウンマネージャーの更新
	skillCooldownManager_->Update();

	//現在のスキルペアのセットを取得
	std::pair<SkillParameters, SkillParameters> currentSkillPairSet = skillPairSets_[activeSkillSetIndex_];

	//スキルのクールダウンの時間を取得
	float skillCooldownTime1 = skillCooldownManager_->GetCooldownTime(currentSkillPairSet.first.name);
	float skillCooldownTime2 = skillCooldownManager_->GetCooldownTime(currentSkillPairSet.second.name);

	//スキルのUIを更新
	skillUI_[activeSkillSetIndex_].first->SetElapsedTime(skillCooldownTime1);
	skillUI_[activeSkillSetIndex_].second->SetElapsedTime(skillCooldownTime2);
}

void Player::UpdateDamageEffect()
{
	//エフェクトがアクティブな場合のみ処理
	if (damageEffect_.color.w > 0.0f)
	{
		//エフェクトタイマーを更新
		damageEffect_.timer += GameTimer::GetDeltaTime() * timeScale_;

		//透明度を計算
		float fadeOutProgress = damageEffect_.timer / damageEffect_.duration;
		damageEffect_.color.w = std::max<float>(0.0f, 0.2f * (1.0f - fadeOutProgress));

		//エフェクトが完全に消えた場合、タイマーと色をリセット
		if (damageEffect_.color.w == 0.0f)
		{
			damageEffect_.timer = 0.0f;
		}
	}

	//ダメージのスプライトの色を設定
	damageEffect_.sprite->SetColor(damageEffect_.color);
}

void Player::UpdateQTEElements()
{
	//定数を定義
	static const Vector2 kCenterPositionFactor = { 0.5f, 0.5f };

	//QTE処理用のデータを取得
	const std::vector<AbstractCharacterState::ProcessedQTEData>& processedQteDatas = currentState_->GetProcessedQTEData();

	//QTEがない場合は処理を飛ばす
	if (processedQteDatas.empty()) return;

	//アクティブ中のQTE処理用データ
	std::vector<AbstractCharacterState::ProcessedQTEData> activeProcessedQteDatas{};

	//アクティブ中のQTEの数を取得
	for (const AbstractCharacterState::ProcessedQTEData& processedQteData : processedQteDatas)
	{
		//QTEのアクション名が設定されていなければ処理を飛ばす
		if (processedQteData.qteActionName.empty()) continue;

		//描画フラグを設定
		bool isVisible = processedQteData.isQTEActive;

		//UIの描画フラグを一括設定
		auto& qteUI = qteUIMap_[processedQteData.qteActionName];
		qteUI.buttonUI.ui->SetIsVisible(isVisible);
		qteUI.actionUI.ui->SetIsVisible(isVisible);
		qteUI.barUI.ui->SetIsVisible(isVisible);

		//描画フラグが有効な場合は配列に追加
		if (isVisible)
		{
			activeProcessedQteDatas.push_back(processedQteData);
		}
	}

	//画面の中心X座標
	Vector2 centerPosition = { Application::kClientWidth * kCenterPositionFactor.x ,Application::kClientHeight * kCenterPositionFactor.y };

	//全てのアクティブ中のQTEのUIを更新
	for (int32_t i = 0; i < activeProcessedQteDatas.size(); i += 2)
	{
		//アクティブ中のQTEの数が一つの場合
		if (activeProcessedQteDatas.size() == 1)
		{
			//QTEのUIを設定
			SetQTEUIPositionAndTime(qteUIMap_[activeProcessedQteDatas[i].qteActionName], centerPosition, activeProcessedQteDatas[i].duration, activeProcessedQteDatas[i].elapsedTime);
			break;
		}

		//一つ目のQTEUIを設定
		Vector2 position = { centerPosition.x - qteUiDistance_ - (qteUiDistance_ * static_cast<float>(i)), centerPosition.y };
		SetQTEUIPositionAndTime(qteUIMap_[activeProcessedQteDatas[i].qteActionName], position, activeProcessedQteDatas[i].duration, activeProcessedQteDatas[i].elapsedTime);

		//二つ目のQTEUIを設定
		if (i + 1 < activeProcessedQteDatas.size())
		{
			position = { centerPosition.x + qteUiDistance_ + (qteUiDistance_ * static_cast<float>(i)), centerPosition.y };
			SetQTEUIPositionAndTime(qteUIMap_[activeProcessedQteDatas[i + 1].qteActionName], position, activeProcessedQteDatas[i + 1].duration, activeProcessedQteDatas[i + 1].elapsedTime);
		}
	}
}

void Player::SetQTEUIPositionAndTime(QTEUI& qteUI, const Vector2& position, float duration, float elapsedTime)
{
	//ボタンUIの座標を設定
	qteUI.buttonUI.ui->SetPosition(position + qteUI.buttonUI.basePosition);

	//アクション名UIの座標を設定
	qteUI.actionUI.ui->SetPosition(position + qteUI.actionUI.basePosition);

	//受付時間バーの座標を設定
	qteUI.barUI.ui->SetPosition(position + qteUI.barUI.basePosition);

	//受付時間バーの受付時間を設定
	qteUI.barUI.ui->SetTimeRemaining(duration);

	//受付時間バーの経過時間を設定
	qteUI.barUI.ui->SetElapsedTime(elapsedTime);
}

bool Player::CheckFallingAttack()
{
	//XとAが同時に押されているか確認
	if (buttonStates_[Player::ButtonType::X].isPressed && buttonStates_[Player::ButtonType::A].isPressed)
	{
		//落下攻撃フラグを設定
		SetActionFlag(ActionFlag::kFallingAttackEnabled, true);
		//落下攻撃発動
		return true;
	}
	//落下攻撃不発動
	return false;
}

bool Player::IsAbilityUsable(const SkillParameters& skill) const
{
	//現在のスキルペアセットインデックスが同じかどうかを確認
	if (activeSkillSetIndex_ != skill.skillSetIndex) return false;

	//クールダウン完了を確認
	if (!GetIsCooldownComplete(skill.name)) return false;

	//地面専用スキルの条件を確認
	if (skill.canUseOnGroundOnly && GetPosition().y > GetAdjustGroundLevel()) return false;

	//アビリティが使用可能
	return true;
}