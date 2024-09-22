#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateStun.h"
#include "Application/Src/Object/Player/States/PlayerStateDeath.h"

void Player::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//インスタンスの取得
	InitializeInstances();

	//スキルクールダウンマネージャーの設定
	InitializeSkillCooldownManager();

	//パーティクルシステムの初期化
	InitializeParticleSystems();

	//コンポーネントの初期化
	InitializeTransformComponent();
	InitializeModelComponent();
	InitializeAnimatorComponent();
	InitializeColliderComponent();

	//状態の初期化
	InitializeState();

	//UIスプライトの生成
	InitializeUISprites();

	//ダメージエフェクト用のスプライトの初期化
	InitializeDamageEffect();

	//音声データの読み込み
	LoadAudioData();

	//環境変数の設定
	ConfigureGlobalVariables();
}

void Player::Update()
{
	//タイトルシーンにいない場合
	if (!isInTitleScene_)
	{
		//状態の更新
		state_->Update();
	}

	//アニメーション後の座標を代入
	preAnimationHipPosition_ = GetHipLocalPosition();

	//スキルのクールダウンの更新
	UpdateSkillCooldowns();

	//回転処理
	UpdateRotation();

	//コライダーの更新
	UpdateCollider();

	//移動制限の処理
	RestrictPlayerMovement(100.0f);

	//体力の更新
	UpdateHP();

	//死亡状態かどうかを確認する
	CheckAndTransitionToDeath();

	//ダメージエフェクトの更新
	UpdateDamageEffect();

	//デバッグ時の更新処理
	DebugUpdate();

	//基底クラスの更新
	GameObject::Update();

	//環境変数の適用
	ApplyGlobalVariables();

	//ImGuiの更新
	UpdateImGui();
}

void Player::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
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

	//体力バーの描画
	hpSprite_->Draw();
	hpFrameSprite_->Draw();

	//ダメージエフェクトのスプライトを描画
	damageEffectSprite_->Draw();
}

void Player::OnCollision(GameObject* gameObject)
{
	//状態の衝突判定処理
	state_->OnCollision(gameObject);

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
}

void Player::OnCollisionEnter(GameObject* gameObject)
{
}

void Player::OnCollisionExit(GameObject* gameObject)
{
}

void Player::ChangeState(IPlayerState* newState)
{
	//新しい状態の設定
	newState->SetPlayer(this);

	//新しい状態の初期化
	newState->Initialize();

	//現在の状態を新しい状態に更新
	state_.reset(newState);
}

void Player::Move(const Vector3& velocity)
{
	transform_->worldTransform_.translation_ += velocity * GameTimer::GetDeltaTime();
}

void Player::ApplyKnockback()
{
	if (knockbackSettings_.knockbackDuration > 0.0f)
	{
		//現在の速度が逆向きにならないように加速度を調整
		if (Mathf::Dot(knockbackSettings_.knockbackVelocity, knockbackSettings_.knockbackAcceleration) > 0.0f)
		{
			//速度と加速度の向きが逆の場合加速度をリセット
			knockbackSettings_.knockbackAcceleration = { 0.0f,0.0f,0.0f };
		}

		//重力加速度を考慮
		Vector3 gravity = { 0.0f, jumpParameters_.gravityAcceleration, 0.0f };
		knockbackSettings_.knockbackVelocity += (gravity + knockbackSettings_.knockbackAcceleration) * GameTimer::GetDeltaTime();

		//ノックバックの持続時間を減少
		knockbackSettings_.knockbackDuration -= GameTimer::GetDeltaTime();

		//移動処理
		Move(knockbackSettings_.knockbackVelocity);

		//持続時間が終了したらノックバックを停止
		if (knockbackSettings_.knockbackDuration <= 0.0f)
		{
			//ノックバックの設定をクリア
			knockbackSettings_ = KnockbackSettings({});
		}
	}
}

void Player::Rotate(const Vector3& vector)
{
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
	destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}

void Player::LookAtEnemy()
{
	//プレイヤーの腰のジョイントのワールド座標を取得
	Vector3 playerPosition = GetHipWorldPosition();

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("")->GetHipWorldPosition();

	//敵の方向へのベクトルを計算
	Vector3 rotateVector = Mathf::Normalize(enemyPosition - playerPosition);

	//Y軸にだけ回転させたいのでY成分を0にする
	rotateVector.y = 0.0f;

	//回転処理
	Rotate(Mathf::Normalize(rotateVector));
}

void Player::CorrectAnimationOffset()
{
	Vector3 hipPositionOffset = GetHipLocalPosition() - preAnimationHipPosition_;
	hipPositionOffset.y = 0.0f;
	SetPosition(GetPosition() - hipPositionOffset);
}

void Player::HandleIncomingDamage(const Weapon* weapon, const bool transitionToStun)
{
	//ノックバックの速度を設定
	knockbackSettings_ = weapon->GetKnockbackSettings();

	//HPを減らす
	hp_ -= weapon->GetDamage();

	//ダメージの音を再生
	audio_->PlayAudio(damageAudioHandle_, false, 0.2f);

	//ダメージエフェクトのタイマーと色を設定
	damageEffectTimer_ = 0.0f;
	damageEffectColor_.w = 0.2f;

	//スタン状態に遷移するかどうかをチェック
	if (transitionToStun)
	{
		ChangeState(new PlayerStateStun());
	}
}

void Player::PlayAnimation(const std::string& name, const float speed, const bool loop)
{
	animator_->PlayAnimation(name, speed, loop);
}

void Player::StopAnimation()
{
	animator_->StopAnimation();
}

void Player::PauseAnimation()
{
	animator_->PauseAnimation();
}

void Player::ResumeAnimation()
{
	animator_->ResumeAnimation();
}

void Player::SetIsAnimationBlending(const bool isBlending)
{
	animator_->SetIsBlending(isBlending);
}

void Player::SetAnimationSpeed(const float animationSpeed)
{
	animator_->SetCurrentAnimationSpeed(animationSpeed);
}

float Player::GetCurrentAnimationTime()
{
	return animator_->GetCurrentAnimationTime();
}

void Player::SetCurrentAnimationTime(const float animationTime)
{
	animator_->SetCurrentAnimationTime(animationTime);
}

float Player::GetNextAnimationTime()
{
	return animator_->GetNextAnimationTime();
}

void Player::SetNextAnimationTime(const float animationTime)
{
	animator_->SetNextAnimationTime(animationTime);
}

bool Player::GetIsAnimationFinished()
{
	return animator_->GetIsAnimationFinished();
}

bool Player::GetIsBlendingCompleted()
{
	return animator_->GetIsBlendingCompleted();
}

float Player::GetCurrentAnimationDuration()
{
	return animator_->GetCurrentAnimationDuration();
}

float Player::GetNextAnimationDuration()
{
	return animator_->GetNextAnimationDuration();
}

float Player::GetCurrentAnimationSpeed()
{
	return animator_->GetCurrentAnimationSpeed();
}

Vector3 Player::GetHipLocalPosition()
{
	//腰のジョイントのローカル座標を計算
	Vector3 hipLocalPosition = GetHipWorldPosition() - transform_->GetWorldPosition();

	//腰のジョイントのローカル座標を返す
	return hipLocalPosition;
}

Vector3 Player::GetHipWorldPosition()
{
	//腰のジョイントのワールドトランスフォームを取得
	WorldTransform hipWorldTransform = model_->GetModel()->GetJointWorldTransform("mixamorig:Hips");

	//ワールド座標を取得しVector3に変換して返す
	return Vector3{
		hipWorldTransform.matWorld_.m[3][0],
		hipWorldTransform.matWorld_.m[3][1],
		hipWorldTransform.matWorld_.m[3][2],
	};
}

Vector3 Player::GetPosition()
{
	return transform_->worldTransform_.translation_;
}

void Player::SetPosition(const Vector3& position)
{
	transform_->worldTransform_.translation_ = position;
}

bool Player::GetActionFlag(const ActionFlag& flag) const
{
	auto it = flags_.find(flag);
	return it != flags_.end() && it->second;
}

const bool Player::GetIsCooldownComplete(const Skill& skill) const
{
	return skillCooldownManager_->IsCooldownComplete(skill);
}

void Player::ResetCooldown(const Skill& skill)
{
	skillCooldownManager_->ResetCooldown(skill);
}

void Player::AddParticleEmitter(const std::string& name, ParticleEmitter* particleEmitter)
{
	//パーティクルシステムを検索する
	auto it = particleSystems_.find(name);
	if (it != particleSystems_.end())
	{
		//パーティクルシステムにエミッターを追加する
		if (it->second != nullptr)
		{	
			it->second->AddParticleEmitter(particleEmitter);
		}
	}
}

void Player::RemoveParticleEmitter(const std::string& particleName, const std::string& emitterName)
{
	//パーティクルシステムを検索する
	auto it = particleSystems_.find(particleName);
	if (it != particleSystems_.end())
	{
		//パーティクルシステムのエミッターを削除する
		if (it->second)
		{
			it->second->RemoveParticleEmitter(emitterName);
		}
	}
}

void Player::AddAccelerationField(const std::string& name, AccelerationField* accelerationField)
{
	//パーティクルシステムを検索する
	auto it = particleSystems_.find(name);
	if (it != particleSystems_.end())
	{
		//パーティクルシステムに加速フィールドを追加する
		if (it->second != nullptr)
		{
			it->second->AddAccelerationField(accelerationField);
		}
	}
}

void Player::RemoveAccelerationField(const std::string& particleName, const std::string& accelerationFieldName)
{
	//パーティクルシステムを検索する
	auto it = particleSystems_.find(particleName);
	if (it != particleSystems_.end())
	{
		//パーティクルシステムの加速フィールドを削除する
		if (it->second)
		{
			it->second->RemoveAccelerationField(accelerationFieldName);
		}
	}
}

ParticleSystem* Player::GetParticleSystem(const std::string& name) const
{
	//パーティクルシステムを検索する
	auto it = particleSystems_.find(name);
	if (it != particleSystems_.end())
	{
		//パーティクルシステムを返す
		return it->second;
	}
	//見つからなかった場合nullptrを返す
	return nullptr;
}

void Player::InitializeInstances()
{
	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
}

void Player::InitializeSkillCooldownManager()
{
	skillCooldownManager_ = std::make_unique<SkillCooldownManager>();
	skillCooldownManager_->AddSkill(Skill::kLaunchAttack, &launchAttackParameters_);
	skillCooldownManager_->AddSkill(Skill::kSpinAttack, &spinAttackParameters_);
}

void Player::InitializeParticleSystems()
{
	//テクスチャの読み込み
	TextureManager::Load("smoke_01.png");

	//パーティクルシステムの生成
	particleSystems_["Smoke"] = ParticleManager::Create("Smoke");
	particleSystems_["Smoke"]->SetTexture("smoke_01.png");
}

void Player::InitializeDamageEffect()
{
	//ダメージエフェクト用のスプライトの生成
	damageEffectSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	damageEffectSprite_->SetColor(damageEffectColor_);
	damageEffectSprite_->SetTextureSize({ 1280.0f,720.0f });
}

void Player::InitializeState()
{
	ChangeState(new PlayerStateRoot());
}

void Player::InitializeTransformComponent()
{
	transform_ = GetComponent<TransformComponent>();
	transform_->worldTransform_.rotationType_ = RotationType::Quaternion;
	destinationQuaternion_ = transform_->worldTransform_.quaternion_;
}

void Player::InitializeModelComponent()
{
	model_ = GetComponent<ModelComponent>();
	for (size_t i = 0; i < model_->GetModel()->GetNumMaterials(); ++i)
	{
		model_->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
	}
}

void Player::InitializeAnimatorComponent()
{
	animator_ = GetComponent<AnimatorComponent>();
}

void Player::InitializeColliderComponent()
{
	collider_ = GetComponent<AABBCollider>();
}

void Player::InitializeUISprites()
{
	LoadAndCreateSprite("HpBar.png", hpSprite_, hpSpritePosition_, { 0.0f, 1.0f, 0.0f, 1.0f });
	LoadAndCreateSprite("HpBarFrame.png", hpFrameSprite_, hpFrameSpritePosition_, { 0.0f, 1.0f, 0.0f, 1.0f });

	for (int32_t i = 0; i < kMaxButtons; ++i)
	{
		SetButtonUISprite(buttonUISettings_[i], buttonConfigs[i]);
	}

	for (int32_t i = 0; i < kMaxSkillCount; ++i)
	{
		SetSkillUISprite(skillUISettings_[i], skillConfigs[i]);
	}
}

void Player::LoadAudioData()
{
	damageAudioHandle_ = audio_->LoadAudioFile("Damage.mp3");
}

void Player::ConfigureGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "ColliderOffset", colliderOffset_);
}

void Player::LoadAndCreateSprite(const std::string& textureFileName, std::unique_ptr<Sprite>& sprite, const Vector2& position, const Vector4& color)
{
	TextureManager::Load(textureFileName);
	sprite.reset(Sprite::Create(textureFileName, position));
	sprite->SetColor(color);
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

void Player::UpdateRotation()
{
	//現在のクォータニオンと目的のクォータニオンを補間
	transform_->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transform_->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
}

void Player::UpdateCollider()
{
	//腰のジョイントの位置を取得
	Vector3 hipPosition = GetHipLocalPosition();

	//アニメーション補正がアクティブな場合、腰のY成分だけ補正
	if (isAnimationCorrectionActive_)
	{
		//補正フラグをリセット
		isAnimationCorrectionActive_ = false;

		//腰の位置からXとZ成分をゼロにして補正
		hipPosition = { 0.0f, hipPosition.y, 0.0f };
	}

	//コライダーの中心座標を設定
	collider_->SetCenter(hipPosition + colliderOffset_);
}

void Player::RestrictPlayerMovement(float moveLimit)
{
	//プレイヤーの現在位置から原点までの距離を計算
	float distance = Mathf::Length(transform_->worldTransform_.translation_);

	//距離が移動制限を超えているかどうかを確認
	if (distance > moveLimit) {
		//スケールを計算して移動制限範囲に収めるよう位置を調整
		float scale = moveLimit / distance;
		transform_->worldTransform_.translation_ *= scale;
	}
}

void Player::UpdateHP()
{
	//HPが0を下回らないようにする
	hp_ = std::max<float>(hp_, 0.0f);

	//体力バーの更新
	UpdateHealthBar();

	//ビネットエフェクトの更新
	UpdateVignetteEffects();
}

void Player::UpdateHealthBar()
{
	//現在の体力バーのサイズを計算して設定
	Vector2 currentHPSize = { hpSpriteSize_.x * (hp_ / kMaxHP), hpSpriteSize_.y };
	hpSprite_->SetSize(currentHPSize);
}

void Player::DrawSkillUI(const SkillUISettings& uiSettings)
{
	uiSettings.buttonSettings.buttonSprite.sprite->Draw();
	uiSettings.buttonSettings.fontSprite.sprite->Draw();
	uiSettings.cooldownBarSprite->Draw();
}

void Player::DrawButtonUI(const ButtonUISettings& uiSettings)
{
	uiSettings.buttonSprite.sprite->Draw();
	uiSettings.fontSprite.sprite->Draw();
}

void Player::UpdateVignetteEffects()
{
	//プレイヤーの体力が一定量以下になったらVignetteをかける
	const float lowHealthThresholdRatio = 4.0f;
	bool shouldEnableVignette = (hp_ <= kMaxHP / lowHealthThresholdRatio);
	PostEffects::GetInstance()->GetVignette()->SetIsEnable(shouldEnableVignette);
}

void Player::UpdateDamageEffect()
{
	//エフェクトがアクティブな場合のみ処理
	if (damageEffectColor_.w > 0.0f)
	{
		//エフェクトタイマーを更新
		damageEffectTimer_ += GameTimer::GetDeltaTime();

		//透明度を計算
		float fadeOutProgress = damageEffectTimer_ / damageEffectDuration_;
		damageEffectColor_.w = std::max<float>(0.0f, 0.2f * (1.0f - fadeOutProgress));

		//エフェクトが完全に消えた場合、タイマーと色をリセット
		if (damageEffectColor_.w == 0.0f)
		{
			damageEffectTimer_ = 0.0f;
		}
	}

	//ダメージのスプライトの色を設定
	damageEffectSprite_->SetColor(damageEffectColor_);
}

void Player::CheckAndTransitionToDeath()
{
	//敵の体力が0を下回っていた場合
	if (hp_ <= 0.0f)
	{
		//死亡状態に遷移
		if (!isDead_)
		{
			ChangeState(new PlayerStateDeath());
		}

		//死亡フラグを立てる
		isDead_ = true;
	}
}

void Player::DebugUpdate()
{
	//デバッグ状態の場合
	if (isDebug_)
	{
		//アニメーションの時間を設定
		animator_->SetAnimationTime(currentAnimationName_, animationTime_);

		//アニメーションのブレンドを無効化する
		animator_->SetIsBlending(false);
	}
	else
	{
		//アニメーションのブレンドを有効化する
		animator_->SetIsBlending(true);
	}
}

void Player::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	colliderOffset_ = globalVariables->GetVector3Value(groupName, "ColliderOffset");
}

void Player::UpdateImGui()
{
	//ImGuiのウィンドウを開始
	ImGui::Begin("Player");

	//デバッグモードのチェックボックス
	ImGui::Checkbox("IsDebug", &isDebug_);

	//デバッグ状態の場合
	if (isDebug_)
	{
		//アニメーション時間のスライダー
		ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f, 0.0f, animator_->GetAnimation(currentAnimationName_)->GetDuration());

		//アニメーション名のコンボボックス
		if (ImGui::BeginCombo("AnimationName", currentAnimationName_.c_str()))
		{
			//アニメーション名のリストを表示
			for (const auto& animation : animationName_)
			{
				//現在のアニメーション名とリストのアニメーションが一致するか確認
				bool isSelected = (currentAnimationName_ == animation);

				//アニメーション名を選択できる項目として表示
				if (ImGui::Selectable(animation.c_str(), isSelected))
				{
					//選択されたアニメーション名を現在のアニメーション名として設定
					currentAnimationName_ = animation;
					animator_->PlayAnimation(currentAnimationName_, 1.0f, true);
				}

				//選択された項目にフォーカスを設定
				if (isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}
			ImGui::EndCombo();
		}
	}

	//ImGuiのウィンドウを終了
	ImGui::End();
}