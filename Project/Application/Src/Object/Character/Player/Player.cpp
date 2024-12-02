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

	//UIの更新
	UpdateUI();

	//UIの編集
	EditUI();

	//基底クラスの更新
	BaseCharacter::Update();

	//QTEの要素の更新
	UpdateQTEElements();
}

void Player::DrawUI()
{
	//UIの数だけループ
	for (int32_t i = 0; i < kMaxActionCount; ++i)
	{
		//通常のボタンUIを描画
		DrawButtonUI(buttonUISettings_[i]);
	}

	//スキルの数だけループ
	for (int32_t i = 0; i < skillConfigs_.size(); ++i)
	{
		//スキルのUIを描画
		DrawSkillUI(skillUISettings_[i]);
	}

	//QTEの数だけループ
	for (const auto& pair : qteButtonUISettings_)
	{
		//QTEのUIを描画
		DrawQTEUI(pair.second);
	}

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

	//ダメージの音を再生
	audio_->PlayAudio(audioHandles_["Damage"], false, 0.2f);

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
	//アクションマップの初期化
	actionMap_ = {
		{"None", ActionCondition{[this]() {return true; }, [this]() {return true; }}},
		{"Move", ActionCondition{[this]() {return true; }, [this]() { return Mathf::Length({ input_->GetLeftStickX(), 0.0f, input_->GetLeftStickY() }) > rootParameters_.walkThreshold; }}},
		{"Jump", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::A].isTriggered && (GetPosition().y == 0.0f || GetActionFlag(Player::ActionFlag::kCanStomp)); }}},
		{"Dodge", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::RB].isTriggered; }}},
		{"Dash", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::LB].isTriggered; }}},
		{"Attack", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::X].isTriggered; }}},
		{"Magic", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::LT].isReleased && actionFlags_[Player::ActionFlag::kMagicAttackEnabled]; }}},
		{"ChargeMagic", ActionCondition{[this]() {return true; }, [this]() { return buttonStates_[Player::ButtonType::LT].isReleased && actionFlags_[Player::ActionFlag::kChargeMagicAttackEnabled]; }}},
		{"FallingAttack", ActionCondition{[this]() {return true; }, [this]() {return CheckFallingAttack(); }}},
		{"Ability1", ActionCondition{[this]() {return IsAbilityUsable(skillPairSets_[activeSkillSetIndex_].first); }, [this]() {return buttonStates_[Player::ButtonType::Y].isTriggered; }}},
		{"Ability2", ActionCondition{[this]() {return IsAbilityUsable(skillPairSets_[activeSkillSetIndex_].second); }, [this]() {return buttonStates_[Player::ButtonType::B].isTriggered; }}},
	};
}

void Player::InitializeAudio()
{
	//基底クラスの呼び出し
	BaseCharacter::InitializeAudio();

	//音声データの読み込み
	audioHandles_["NormalHit"] = audio_->LoadAudioFile("Hit.mp3");
	audioHandles_["Dash"] = audio_->LoadAudioFile("Dash.mp3");
	audioHandles_["Damage"] = audio_->LoadAudioFile("Damage.mp3");
}

void Player::InitializeAnimator()
{
	//基底クラスの呼び出し
	BaseCharacter::InitializeAnimator();

	//アニメーション名とファイルパス
	std::vector<std::pair<std::string, std::string>> animations = {
		{"Idle", "Player/Animations/Idle.gltf"}, {"Walk1", "Player/Animations/Walk1.gltf"}, {"Walk2", "Player/Animations/Walk2.gltf"},
		{"Walk3", "Player/Animations/Walk3.gltf"}, {"Walk4", "Player/Animations/Walk4.gltf"}, {"Run1", "Player/Animations/Run1.gltf"},
		{"Run2", "Player/Animations/Run2.gltf"}, {"Run3", "Player/Animations/Run3.gltf"}, {"Run4", "Player/Animations/Run4.gltf"},
		{"Jump1", "Player/Animations/Jump1.gltf"}, {"Jump2", "Player/Animations/Jump2.gltf"}, {"DodgeForward", "Player/Animations/DodgeForward.gltf"},
		{"DodgeBackward", "Player/Animations/DodgeBackward.gltf"}, {"DashStart", "Player/Animations/DashStart.gltf"}, {"DashEnd", "Player/Animations/DashEnd.gltf"},
		{"Falling", "Player/Animations/Falling.gltf"}, {"GroundAttack1", "Player/Animations/GroundAttack1.gltf"},  {"GroundAttack2", "Player/Animations/GroundAttack2.gltf"},
		{"GroundAttack3", "Player/Animations/GroundAttack3.gltf"}, {"GroundAttack4", "Player/Animations/GroundAttack4.gltf"}, {"AerialAttack1", "Player/Animations/AerialAttack1.gltf"},
		{"AerialAttack2", "Player/Animations/AerialAttack2.gltf"}, {"AerialAttack3", "Player/Animations/AerialAttack3.gltf"}, {"LaunchAttack", "Player/Animations/LaunchAttack.gltf"},
		{"SpinAttack", "Player/Animations/SpinAttack.gltf"}, {"FallingAttack", "Player/Animations/FallingAttack.gltf"}, {"Casting", "Player/Animations/Casting.gltf"}, {"MagicAttack", "Player/Animations/MagicAttack.gltf"}, 
		{"HitStun", "Player/Animations/HitStun.gltf"}, {"Knockdown", "Player/Animations/Knockdown.gltf"}, {"StandUp", "Player/Animations/StandUp.gltf"},{"Death", "Player/Animations/Death.gltf"},
		{"JustDodge", "Player/Animations/JustDodge.gltf"}, {"Counter", "Player/Animations/Counter.gltf"},
	};

	//アニメーションを追加
	for (const auto& [name, path] : animations)
	{
		animator_->AddAnimation(name, AnimationManager::Create(path));
	}

	//通常アニメーションを再生
	animator_->PlayAnimation("Idle", 1.0f, true);
}

void Player::InitializeUISprites()
{
	//テクスチャの名前を設定
	hpTextureNames_ = { {
		{"barBack_horizontalLeft.png", "barBack_horizontalMid.png", "barBack_horizontalRight.png"},
		{"barRed_horizontalLeft.png", "barRed_horizontalMid.png", "barRed_horizontalRight.png"},}
	};

	//スプライトの座標を設定
	hpBarSegmentPositions_ = { {
		{ {	{71.0f, 40.0f}, {80.0f, 40.0f}, {560.0f, 40.0f},}},
		{ { {71.0f, 40.0f}, {80.0f, 40.0f}, {560.0f, 40.0f},}},}
	};

	//ボタンのUIの設定
	for (int32_t i = 0; i < kMaxActionCount; ++i)
	{
		SetButtonUISprite(buttonUISettings_[i], buttonConfigs_[i]);
	}

	//スキルのUIの設定
	for (int32_t i = 0; i < kMaxSkillCount; ++i)
	{
		SetSkillUISprite(skillUISettings_[i], skillConfigs_[i]);
	}

	//テクスチャ名
	std::map<std::string, std::pair<std::string, std::string>> textureNames = {{"None", {"white.png", "white.png"}}, {"Move", {"white.png", "white.png"}},{"Jump", {"xbox_button_a_outline.png", "Jump.png"}},
		{"Dodge", {"xbox_rb_outline.png", "Dodge.png"}},{"Dash", {"xbox_lb_outline.png", "Dash.png"}},{"Attack", {"xbox_button_x_outline.png", "Attack.png"}},{"Magic", {"xbox_lt_outline.png", "Fire.png"}}, 
		{"ChargeMagic", {"xbox_lt_outline.png", "Fire.png"}},{"FallingAttack", {"xa.png", "FallingAttack.png"}}, {"Ability1", {"xbox_button_y_outline.png", "LaunchAttack.png"}},{"Ability2", {"xbox_button_b_outline.png", "SpinAttack.png"}},
	};

	//QTEのUI設定
	std::map<std::string, QTEUIConfig> qteUiConfigs = {
		{"None", { {0.0f, 21.0f}, {1.0f, 1.0f}, {0.0f, 21.0f}, {0.4f, 0.4f}, {-64.0f, -21.0f}, {64.0f, 8.0f}}},
		{"Move", { {0.0f, 21.0f}, {1.0f, 1.0f}, {0.0f, 21.0f}, {0.4f, 0.4f}, {-64.0f, -21.0f}, {64.0f, 8.0f}}},
		{"Jump", { {-50.0f, -21.0f}, {1.0f, 1.0f}, {30.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
		{"Dodge", { {-40.0f, -21.0f}, {1.0f, 1.0f}, {30.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
		{"Dash", { {-50.0f, -17.0f}, {1.0f, 1.0f}, {30.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
		{"Attack", { {-36.0f, -21.0f}, {1.0f, 1.0f}, {26.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
		{"Magic", { {-50.0f, -14.0f}, {1.0f, 1.0f}, {30.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
		{"ChargeMagic", { {-50.0f, -14.0f}, {1.0f, 1.0f}, {30.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
		{"FallingAttack", { {-70.0f, -21.0f}, {1.0f, 1.0f}, {50.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
		{"Ability1",{ {-100.0f, -21.0f}, {1.0f, 1.0f}, {30.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
		{"Ability2", { {-70.0f, -21.0f}, {1.0f, 1.0f}, {30.0f, -21.0f}, {0.4f, 0.4f}, {-64.0f, 21.0f}, {64.0f, 8.0f}}},
	};

	//QTEのUI設定を生成
	for (const auto& texturePair : textureNames) 
	{
		//アクションの名前を取得
		const std::string& actionName = texturePair.first;

		//テクスチャ名を取得
		const auto& textures = texturePair.second;

		//QTEのUI構成を追加
		qteConfigs_[actionName] = qteUiConfigs[actionName];

		//QTEのUI設定を追加
		TextureManager::Load(textures.first);
		TextureManager::Load(textures.second);
		qteButtonUISettings_[actionName] = QTEButtonUI{};

		//ボタンスプライトの設定
		qteButtonUISettings_[actionName].buttonSettings.buttonSprite.sprite.reset(Sprite::Create(textures.first, { 0.0f, 0.0f }));
		qteButtonUISettings_[actionName].buttonSettings.buttonSprite.sprite->SetScale(qteConfigs_[actionName].buttonScale);
		qteButtonUISettings_[actionName].buttonSettings.buttonSprite.sprite->SetAnchorPoint({ 0.5f, 0.5f });

		//フォントスプライトの設定
		qteButtonUISettings_[actionName].buttonSettings.fontSprite.sprite.reset(Sprite::Create(textures.second, { 0.0f, 0.0f }));
		qteButtonUISettings_[actionName].buttonSettings.fontSprite.sprite->SetScale(qteConfigs_[actionName].fontScale);
		qteButtonUISettings_[actionName].buttonSettings.fontSprite.sprite->SetAnchorPoint({ 0.5f, 0.5f });

		//QTEバースプライトの設定
		qteButtonUISettings_[actionName].qteBarSettings.sprite.reset(Sprite::Create("white.png", { 0.0f, 0.0f }));
		qteButtonUISettings_[actionName].qteBarSettings.sprite->SetScale(qteConfigs_[actionName].qteBarScale);
	}

	//基底クラスの呼び出し
	BaseCharacter::InitializeUISprites();
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

void Player::SetButtonUISprite(ButtonUISettings& uiSettings, const ButtonConfig& config)
{
	//テクスチャの読み込み
	TextureManager::Load(config.buttonTexture);
	TextureManager::Load(config.fontTexture);

	//ボタンのスプライトの設定
	uiSettings.buttonSprite.position = config.buttonPosition;
	uiSettings.buttonSprite.scale = config.buttonScale;
	uiSettings.buttonSprite.sprite.reset(Sprite::Create(config.buttonTexture, config.buttonPosition));
	uiSettings.buttonSprite.sprite->SetScale(config.buttonScale);
	uiSettings.buttonSprite.sprite->SetAnchorPoint({ 0.5f,0.5f });

	//フォントのスプライトの設定
	uiSettings.fontSprite.position = config.fontPosition;
	uiSettings.fontSprite.scale = config.fontScale;
	uiSettings.fontSprite.sprite.reset(Sprite::Create(config.fontTexture, config.fontPosition));
	uiSettings.fontSprite.sprite->SetScale(config.fontScale);
}

void Player::SetSkillUISprite(SkillUISettings& uiSettings, const SkillConfig& config)
{
	//ボタンのスプライトの設定
	SetButtonUISprite(uiSettings.buttonSettings, config.buttonConfig);

	//クールダウンバーの設定
	uiSettings.cooldownBarSprite.reset(Sprite::Create("white.png", config.skillBarPosition));
	uiSettings.cooldownBarSprite->SetScale(config.skillBarScale);
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

	//クールダウンバーのスケールを更新
	UpdateCooldownBarScale(skillUISettings_[0], skillConfigs_[0], skillCooldownTime1, currentSkillPairSet.first.cooldownDuration);
	UpdateCooldownBarScale(skillUISettings_[1], skillConfigs_[1], skillCooldownTime2, currentSkillPairSet.second.cooldownDuration);
}

void Player::UpdateCooldownBarScale(SkillUISettings& uiSettings, const SkillConfig& config, float cooldownTime, float cooldownDuration)
{
	float currentScale = config.skillBarScale.x * (cooldownTime / cooldownDuration);
	uiSettings.cooldownBarSprite->SetScale({ currentScale, config.skillBarScale.y });
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

void Player::UpdateUI()
{
	//アクションボタンのUI更新
	for (int32_t i = 0; i < kMaxActionCount; ++i)
	{
		UpdateButtonScale(buttonStates_[buttonConfigs_[i].buttonType], buttonConfigs_[i].buttonScale, buttonUISettings_[i].buttonSprite, buttonConfigs_[i].buttonType);
	}

	//スキルボタンのUI更新
	for (int32_t i = 0; i < kMaxSkillCount; ++i)
	{
		UpdateButtonScale(buttonStates_[skillConfigs_[i].buttonConfig.buttonType], skillConfigs_[i].buttonConfig.buttonScale, skillUISettings_[i].buttonSettings.buttonSprite, buttonConfigs_[i].buttonType);
	}
}

void Player::UpdateButtonScale(const ButtonState& buttonState, const Vector2& baseScale, SpriteSettings& spriteSetting, const ButtonType buttonType)
{
	//UIスケール拡大の倍率
	const float kScaleMultiplier = 2.0f;
	const float kTriggerScaleMultiplier = 1.3f;

	//トリガーボタンの場合は特別なスケール倍率を適用
	float scaleMultiplier = (buttonType == ButtonType::LT || buttonType == ButtonType::XA) ? kTriggerScaleMultiplier : kScaleMultiplier;

	//スケール補間の目標値を設定
	const Vector2 targetScale = (buttonState.isPressed || buttonState.isTriggered) ? baseScale * scaleMultiplier : baseScale;

	//スケールを補間し適用
	spriteSetting.scale = Mathf::Lerp(spriteSetting.scale, targetScale, 0.1f);
	spriteSetting.sprite->SetScale(spriteSetting.scale);
}

void Player::EditUI()
{
	//UIのImGui開始
	ImGui::Begin("UI");

	//ボタンの編集
	for (int32_t i = 0; i < kMaxActionCount; ++i)
	{
		EditButtonConfig(buttonConfigs_[i], buttonUISettings_[i]);
	}

	//スキルの編集
	for (int32_t i = 0; i < kMaxSkillCount; ++i)
	{
		EditSkillConfig(skillConfigs_[i], skillUISettings_[i], i);
	}

	//QTEの編集
	for (auto& qteConfig : qteConfigs_)
	{
		EditQTEConfig(qteConfig.second, qteButtonUISettings_[qteConfig.first], qteConfig.first);
	}

	//UIのImGui終了
	ImGui::End();
}

void Player::EditButtonConfig(ButtonConfig& config, ButtonUISettings& uiSettings)
{
	//区切り線
	ImGui::Separator();

	//ボタンの座標を編集
	std::string buttonTextureButtonPositionName = config.buttonTexture + " Position";
	ImGui::DragFloat2(buttonTextureButtonPositionName.c_str(), &config.buttonPosition.x, 0.1f);

	//フォントの座標を編集
	std::string buttonTextureFontPositionName = config.fontTexture + " Position";
	ImGui::DragFloat2(buttonTextureFontPositionName.c_str(), &config.fontPosition.x, 0.1f);

	//ボタンとフォントの設定を適用
	uiSettings.buttonSprite.sprite->SetPosition(config.buttonPosition);
	uiSettings.fontSprite.sprite->SetPosition(config.fontPosition);
}

void Player::EditSkillConfig(SkillConfig& config, SkillUISettings& uiSettings, const int32_t index)
{
	//区切り線
	ImGui::Separator();

	//ボタンの設定を編集
	EditButtonConfig(config.buttonConfig, uiSettings.buttonSettings);

	//スキルバーの座標を編集
	std::string skillBarPositionName = "SkillBarPosition" + std::to_string(index);
	ImGui::DragFloat2(skillBarPositionName.c_str(), &config.skillBarPosition.x, 0.1f);

	//スキルバーの設定を適用
	uiSettings.cooldownBarSprite->SetPosition(config.skillBarPosition);
}

void Player::EditQTEConfig(QTEUIConfig& config, QTEButtonUI& uiSettings, const std::string& actionName)
{
	//区切り線
	ImGui::Separator();

	//UIを描画するかどうか
	std::string visibleFlagName = actionName + "IsVisible";
	ImGui::Checkbox(visibleFlagName.c_str(), &qteButtonUISettings_[actionName].isVisible);

	//ボタンの設定を編集
	std::string buttonTextureButtonPositionName = actionName + "ButtonPosition";
	ImGui::DragFloat2(buttonTextureButtonPositionName.c_str(), &config.buttonPosition.x, 0.1f);

	//フォントの座標を編集
	std::string buttonTextureFontPositionName = actionName + "FontPosition";
	ImGui::DragFloat2(buttonTextureFontPositionName.c_str(), &config.fontPosition.x, 0.1f);

	//QTEバーの座標を編集
	std::string qteBarPositionName = actionName + "QTEBarPosition";
	ImGui::DragFloat2(qteBarPositionName.c_str(), &config.qteBarPosition.x, 0.1f);

	//画面の中心X座標
	Vector2 centerPosition = { Application::kClientWidth / 2.0f ,Application::kClientHeight / 2.0f };

	//QTEの設定を適用
	uiSettings.buttonSettings.buttonSprite.sprite->SetPosition(centerPosition + config.buttonPosition);
	uiSettings.buttonSettings.fontSprite.sprite->SetPosition(centerPosition + config.fontPosition);
	uiSettings.qteBarSettings.sprite->SetPosition(centerPosition + config.qteBarPosition);
}

void Player::UpdateQTEElements()
{
	//QTE処理用のデータを取得
	const std::vector<ICharacterState::ProcessedQTEData>& processedQteDatas = currentState_->GetProcessedQTEData();

	//アクティブ中のQTE処理用データ
	std::vector<ICharacterState::ProcessedQTEData> activeProcessedQteDatas{};

	//アクティブ中のQTEの数を取得
	for (const ICharacterState::ProcessedQTEData& processedQteData : processedQteDatas)
	{
		//UIの描画フラグを設定
		qteButtonUISettings_[processedQteData.qteActionName].isVisible = processedQteData.isQTEActive;
		//QTEがアクティブ状態の場合は配列に追加
		if (processedQteData.isQTEActive)
		{
			activeProcessedQteDatas.push_back(processedQteData);
		}
	}

	//画面の中心X座標
	Vector2 centerPosition = { Application::kClientWidth / 2.0f ,Application::kClientHeight / 2.0f };

	//全てのアクティブ中のQTEのUIを更新
	for (int32_t i = 0; i < activeProcessedQteDatas.size(); i += 2)
	{
		//アクティブ中のQTEの数が一つの場合
		if (activeProcessedQteDatas.size() == 1)
		{
			qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.buttonSprite.position = centerPosition + qteConfigs_[activeProcessedQteDatas[i].qteActionName].buttonPosition;
			qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.fontSprite.position = centerPosition + qteConfigs_[activeProcessedQteDatas[i].qteActionName].fontPosition;
			qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].qteBarSettings.position = centerPosition + qteConfigs_[activeProcessedQteDatas[i].qteActionName].qteBarPosition;
			qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.buttonSprite.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.buttonSprite.position);
			qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.fontSprite.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.fontSprite.position);
			qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].qteBarSettings.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].qteBarSettings.position);
			float currentScale = qteConfigs_[activeProcessedQteDatas[i].qteActionName].qteBarScale.x * (1.0f - activeProcessedQteDatas[i].elapsedTime / activeProcessedQteDatas[i].duration);
			qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].qteBarSettings.sprite->SetScale({ currentScale, qteConfigs_[activeProcessedQteDatas[i].qteActionName].qteBarScale.y });
			break;
		}

		//左側にUIを移動
		qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.buttonSprite.position = { centerPosition.x - qteUiDistance_ + (i / 2 * qteUiDistance_) + qteConfigs_[activeProcessedQteDatas[i].qteActionName].buttonPosition.x, centerPosition.y + qteConfigs_[activeProcessedQteDatas[i].qteActionName].buttonPosition.y };
		qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.fontSprite.position = { centerPosition.x - qteUiDistance_ + (i / 2 * qteUiDistance_) + qteConfigs_[activeProcessedQteDatas[i].qteActionName].fontPosition.x, centerPosition.y + qteConfigs_[activeProcessedQteDatas[i].qteActionName].fontPosition.y };
		qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].qteBarSettings.position = { centerPosition.x - qteUiDistance_ + (i / 2 * qteUiDistance_) + qteConfigs_[activeProcessedQteDatas[i].qteActionName].qteBarPosition.x, centerPosition.y + qteConfigs_[activeProcessedQteDatas[i].qteActionName].qteBarPosition.y };

		//QTEのボタンUIの設定を更新
		qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.buttonSprite.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.buttonSprite.position);
		qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.fontSprite.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].buttonSettings.fontSprite.position);
		qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].qteBarSettings.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].qteBarSettings.position);
		float currentScale1 = qteConfigs_[activeProcessedQteDatas[i].qteActionName].qteBarScale.x * (1.0f - activeProcessedQteDatas[i].elapsedTime / activeProcessedQteDatas[i].duration);
		qteButtonUISettings_[activeProcessedQteDatas[i].qteActionName].qteBarSettings.sprite->SetScale({ currentScale1, qteConfigs_[activeProcessedQteDatas[i].qteActionName].qteBarScale.y });

		//右側にUIを移動
		qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].buttonSettings.buttonSprite.position = { centerPosition.x + qteUiDistance_ + (i / 2 * qteUiDistance_) + qteConfigs_[activeProcessedQteDatas[i + 1].qteActionName].buttonPosition.x, centerPosition.y + qteConfigs_[activeProcessedQteDatas[i + 1].qteActionName].buttonPosition.y };
		qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].buttonSettings.fontSprite.position = { centerPosition.x + qteUiDistance_ + (i / 2 * qteUiDistance_) + qteConfigs_[activeProcessedQteDatas[i + 1].qteActionName].fontPosition.x, centerPosition.y + qteConfigs_[activeProcessedQteDatas[i + 1].qteActionName].fontPosition.y };
		qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].qteBarSettings.position = { centerPosition.x + qteUiDistance_ + (i / 2 * qteUiDistance_) + qteConfigs_[activeProcessedQteDatas[i + 1].qteActionName].qteBarPosition.x, centerPosition.y + qteConfigs_[activeProcessedQteDatas[i + 1].qteActionName].qteBarPosition.y };

		//QTEのボタンUIの設定を更新
		qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].buttonSettings.buttonSprite.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].buttonSettings.buttonSprite.position);
        qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].buttonSettings.fontSprite.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].buttonSettings.fontSprite.position);
        qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].qteBarSettings.sprite->SetPosition(qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].qteBarSettings.position);
		float currentScale2 = qteConfigs_[activeProcessedQteDatas[i + 1].qteActionName].qteBarScale.x * (1.0f - activeProcessedQteDatas[i + 1].elapsedTime / activeProcessedQteDatas[i + 1].duration);
		qteButtonUISettings_[activeProcessedQteDatas[i + 1].qteActionName].qteBarSettings.sprite->SetScale({ currentScale2, qteConfigs_[activeProcessedQteDatas[i + 1].qteActionName].qteBarScale.y });
	}
}

void Player::DrawButtonUI(const ButtonUISettings& uiSettings)
{
	uiSettings.buttonSprite.sprite->Draw();
	uiSettings.fontSprite.sprite->Draw();
}

void Player::DrawSkillUI(const SkillUISettings& uiSettings)
{
	uiSettings.buttonSettings.buttonSprite.sprite->Draw();
	uiSettings.buttonSettings.fontSprite.sprite->Draw();
	uiSettings.cooldownBarSprite->Draw();
}

void Player::DrawQTEUI(const QTEButtonUI& uiSettings)
{
	//描画フラグが立っていない場合は飛ばす
	if (!uiSettings.isVisible) return;

	//UIの描画
	uiSettings.buttonSettings.buttonSprite.sprite->Draw();
	uiSettings.buttonSettings.fontSprite.sprite->Draw();
	uiSettings.qteBarSettings.sprite->Draw();
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
	//クールダウン完了を確認
	if (!GetIsCooldownComplete(skill.name))
	{
		return false;
	}

	//地面専用スキルの条件を確認
	if (skill.canUseOnGroundOnly && GetPosition().y > 0.0f)
	{
		return false;
	}

	//アビリティが使用可能
	return true;
}