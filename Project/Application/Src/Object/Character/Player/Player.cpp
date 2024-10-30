#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"

void Player::Initialize()
{
	//基底クラスの初期化
	BaseCharacter::Initialize();

	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//HPの初期化
	maxHp_ = 100.0f;
	hp_ = maxHp_;

	//スキルクールダウンマネージャーの初期化
	skillCooldownManager_ = std::make_unique<SkillCooldownManager>();
	skillCooldownManager_->AddSkill(Skill::kLaunchAttack, &launchAttackParameters_);
	skillCooldownManager_->AddSkill(Skill::kSpinAttack, &spinAttackParameters_);

	//ダメージエフェクト用のスプライトの生成
	damageEffect_.sprite.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	damageEffect_.sprite->SetColor(damageEffect_.color);
	damageEffect_.sprite->SetTextureSize({ 1280.0f,720.0f });

	//音声データの読み込み
	damageAudioHandle_ = audio_->LoadAudioFile("Damage.mp3");

	//状態の初期化
	ChangeState(new PlayerStateRoot());
}

void Player::Update()
{
	//タイトルシーンにいる場合は基底クラスの更新だけする
	if (isInTitleScene_)
	{
		GameObject::Update();
		return;
	}

	//モデルシェイクでずれた分の座標をリセット
	ResetToOriginalPosition();

	//モデルシェイクの更新
	UpdateModelShake();

	//状態の更新
	state_->Update();

	//魔法攻撃の更新
	UpdateMagicAttack();

	//スキルのクールダウンの更新
	UpdateSkillCooldowns();

	//ダメージエフェクトの更新
	UpdateDamageEffect();

	//基底クラスの更新
	BaseCharacter::Update();
}

void Player::DrawUI()
{
	//右トリガーの値が閾値を超えているかをチェック
	const float kRightTriggerThreshold = 0.7f;
	bool shouldDrawSkillUI = input_->GetRightTriggerValue() > kRightTriggerThreshold;

	//ボタン数だけループ
	for (int32_t i = 0; i < kMaxButtons; ++i)
	{
		//右トリガーの値が閾値を超えていて、かつボタンがXまたはYの場合
		if (shouldDrawSkillUI && (i == X || i == Y))
		{
			//XまたはYボタンに対応するスキルUIを描画
			int skillIndex = (i == X) ? 0 : 1;
			DrawSkillUI(skillUISettings_[skillIndex]);
		}
		else
		{
			//通常のボタンUIを描画
			DrawButtonUI(buttonUISettings_[i]);
		}
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
		state_->OnCollision(gameObject);
	}
}

void Player::ApplyDamageAndKnockback(const KnockbackParameters& knockbackSettings, const float damage, const bool transitionToStun)
{
	//ダメージの音を再生
	audio_->PlayAudio(damageAudioHandle_, false, 0.2f);

	//ダメージエフェクトのタイマーと色を設定
	damageEffect_.timer = 0.0f;
	damageEffect_.color.w = 0.2f;

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
		{"SpinAttack", "Player/Animations/SpinAttack.gltf"}, {"FallingAttack", "Player/Animations/FallingAttack.gltf"}, {"Impact", "Player/Animations/Impact.gltf"},
		{"Death", "Player/Animations/Death.gltf"}, {"Casting", "Player/Animations/Casting.gltf"}, {"MagicAttack", "Player/Animations/MagicAttack.gltf"}
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
	//ボタンのUIの設定
	for (int32_t i = 0; i < kMaxButtons; ++i)
	{
		SetButtonUISprite(buttonUISettings_[i], buttonConfigs[i]);
	}

	//スキルのUIの設定
	for (int32_t i = 0; i < kMaxSkillCount; ++i)
	{
		SetSkillUISprite(skillUISettings_[i], skillConfigs[i]);
	}

	//テクスチャの名前を設定
	hpTextureNames_ = { {
		{"barBack_horizontalLeft.png", "barBack_horizontalMid.png", "barBack_horizontalRight.png"},
		{"barRed_horizontalLeft.png", "barRed_horizontalMid.png", "barRed_horizontalRight.png"},
		}
	};

	//スプライトの座標を設定
	hpBarSegmentPositions_ = { {
		{ {	{71.0f, 40.0f}, {80.0f, 40.0f}, {560.0f, 40.0f},}},
		{ { {71.0f, 40.0f}, {80.0f, 40.0f}, {560.0f, 40.0f},}},
		}
	};

	//基底クラスの呼び出し
	BaseCharacter::InitializeUISprites();
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

void Player::UpdateMagicAttack()
{
	//クールダウンのタイマーを進める
	UpdateCooldownTimer();

	//Yボタン押している場合
	if (input_->IsPressButton(XINPUT_GAMEPAD_Y))
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

void Player::UpdateCooldownTimer()
{
	//クールダウンのタイマーを進める
	if (magicAttackWork_.cooldownTimer > 0.0f)
	{
		magicAttackWork_.cooldownTimer -= GameTimer::GetDeltaTime();
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
	magicAttackWork_.chargeTimer += GameTimer::GetDeltaTime();

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

	const SkillConfig& config1 = skillConfigs[0];
	const SkillConfig& config2 = skillConfigs[1];

	float launchAttackCooldownTime = skillCooldownManager_->GetCooldownTime(Skill::kLaunchAttack);
	float spinAttackCooldownTime = skillCooldownManager_->GetCooldownTime(Skill::kSpinAttack);

	UpdateCooldownBarScale(skillUISettings_[0], config1, launchAttackCooldownTime, launchAttackParameters_.cooldownDuration);
	UpdateCooldownBarScale(skillUISettings_[1], config2, spinAttackCooldownTime, spinAttackParameters_.cooldownDuration);
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
		damageEffect_.timer += GameTimer::GetDeltaTime();

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

void Player::TransitionToStunState()
{
}

void Player::TransitionToDeathState()
{
}