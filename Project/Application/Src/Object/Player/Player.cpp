#include "Player.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Collision/SphereCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void Player::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//Audioのインスタンスを取得
	audio_ = Audio::GetInstance();

	//Stateの初期化
	ChangeState(new PlayerStateRoot());

	//パーティクルを生成
	particleSystem_ = ParticleManager::Create("Player");

	//ダメージエフェクト用のスプライトの生成
	damagedSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	damagedSprite_->SetColor(damagedSpriteColor_);
	damagedSprite_->SetSize({ 1280.0f,720.0f });

	//体力のスプライトの生成
	TextureManager::Load("HpBar.png");
	hpSprite_.reset(Sprite::Create("HpBar.png", hpSpritePosition_));
	hpSprite_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	TextureManager::Load("HpBarFrame.png");
	hpFrameSprite_.reset(Sprite::Create("HpBarFrame.png", hpFrameSpritePosition_));
	hpFrameSprite_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });

	//ボタンのスプライトの生成
	buttonSprites_.resize(kMaxButtons);
	for (int32_t i = 0; i < kMaxButtons; i++)
	{
		switch (i)
		{
		case A:
			TextureManager::Load("xbox_button_a_outline.png");
			TextureManager::Load("Jump.png");
			buttonSprites_[A].buttonPosition = { 1000.0f,630.0f };
			buttonSprites_[A].buttonSprite.reset(Sprite::Create("xbox_button_a_outline.png", buttonSprites_[A].buttonPosition));
			buttonSprites_[A].fontPosition = { 1060.0f,644.0f };
			buttonSprites_[A].fontScale = { 0.3f,0.3f };
			buttonSprites_[A].fontSprite.reset(Sprite::Create("Jump.png", buttonSprites_[A].fontPosition));
			buttonSprites_[A].fontSprite->SetScale(buttonSprites_[A].fontScale);
			break;
		case B:
			TextureManager::Load("xbox_button_b_outline.png");
			TextureManager::Load("Dash.png");
			buttonSprites_[B].buttonPosition = { 1048.0f,582.0f };
			buttonSprites_[B].buttonSprite.reset(Sprite::Create("xbox_button_b_outline.png", buttonSprites_[B].buttonPosition));
			buttonSprites_[B].fontPosition = { 1108.0f,596.0f };
			buttonSprites_[B].fontScale = { 0.3f,0.3f };
			buttonSprites_[B].fontSprite.reset(Sprite::Create("Dash.png", buttonSprites_[B].fontPosition));
			buttonSprites_[B].fontSprite->SetScale(buttonSprites_[B].fontScale);
			break;
		case X:
			TextureManager::Load("xbox_button_x_outline.png");
			TextureManager::Load("Attack.png");
			buttonSprites_[X].buttonPosition = { 952.0f, 582.0f };
			buttonSprites_[X].buttonSprite.reset(Sprite::Create("xbox_button_x_outline.png", buttonSprites_[X].buttonPosition));
			buttonSprites_[X].fontPosition = { 880.0f,596.0f };
			buttonSprites_[X].fontScale = { 0.3f,0.3f };
			buttonSprites_[X].fontSprite.reset(Sprite::Create("Attack.png", buttonSprites_[X].fontPosition));
			buttonSprites_[X].fontSprite->SetScale(buttonSprites_[X].fontScale);
			break;
		case Y:
			TextureManager::Load("xbox_button_y_outline.png");
			TextureManager::Load("Fire.png");
			buttonSprites_[Y].buttonPosition = { 1000.0f,534.0f };
			buttonSprites_[Y].buttonSprite.reset(Sprite::Create("xbox_button_y_outline.png", buttonSprites_[Y].buttonPosition));
			buttonSprites_[Y].fontPosition = { 904.0f,544.0f };
			buttonSprites_[Y].fontScale = { 0.3f,0.3f };
			buttonSprites_[Y].fontSprite.reset(Sprite::Create("Fire.png", buttonSprites_[Y].fontPosition));
			buttonSprites_[Y].fontSprite->SetScale(buttonSprites_[Y].fontScale);
			break;
		case LB:
			TextureManager::Load("xbox_lb_outline.png");
			TextureManager::Load("Lockon.png");
			buttonSprites_[LB].buttonPosition = { 1070.0f,429.0f };
			buttonSprites_[LB].buttonSprite.reset(Sprite::Create("xbox_lb_outline.png", buttonSprites_[LB].buttonPosition));
			buttonSprites_[LB].fontPosition = { 1139.0f,439.0f };
			buttonSprites_[LB].fontScale = { 0.3f,0.3f };
			buttonSprites_[LB].fontSprite.reset(Sprite::Create("Lockon.png", buttonSprites_[LB].fontPosition));
			buttonSprites_[LB].fontSprite->SetScale(buttonSprites_[LB].fontScale);
			break;
		case RB:
			TextureManager::Load("xbox_rb_outline.png");
			TextureManager::Load("Dodge.png");
			buttonSprites_[RB].buttonPosition = { 1070.0f,484.0f };
			buttonSprites_[RB].buttonSprite.reset(Sprite::Create("xbox_rb_outline.png", buttonSprites_[RB].buttonPosition));
			buttonSprites_[RB].fontPosition = { 1139.0f,496.0f };
			buttonSprites_[RB].fontScale = { 0.3f,0.3f };
			buttonSprites_[RB].fontSprite.reset(Sprite::Create("Dodge.png", buttonSprites_[RB].fontPosition));
			buttonSprites_[RB].fontSprite->SetScale(buttonSprites_[RB].fontScale);
			break;
		}
	}

	//音声データの読み込み
	damageAudioHandle_ = audio_->LoadAudioFile("Damage.mp3");
	justDodgeAudioHandle_ = audio_->LoadAudioFile("Dodge.mp3");
	fireAudioHandle_ = audio_->LoadAudioFile("Fire.mp3");
	chargeFireAudioHandle_ = audio_->LoadAudioFile("ChargeFire.mp3");
	dashAudioHandle_ = audio_->LoadAudioFile("Dash.mp3");

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "WalkSpeed", rootParameters_.walkSpeed);
	globalVariables->AddItem(groupName, "WalkThreshold", rootParameters_.walkThreshold);
	globalVariables->AddItem(groupName, "RunSpeed", rootParameters_.runSpeed);
	globalVariables->AddItem(groupName, "RunThreshold", rootParameters_.runThreshold);
	globalVariables->AddItem(groupName, "DodgeSpeed", dodgeParameters_.dodgeSpeed);
	globalVariables->AddItem(groupName, "JumpFirstSpeed", jumpParameters_.firstSpeed);
	globalVariables->AddItem(groupName, "GravityAcceleration", jumpParameters_.gravityAcceleration);
}

void Player::Update()
{
	//タイトルシーンにいる場合は移動処理をスキップ
	if (!isInTitleScene_)
	{
		//Stateの更新
		state_->Update();
	}

	//ダメージを食らったかどうかのフラグをリセット
	isDamaged_ = false;

	//チャージ魔法の更新
	UpdateChargeMagicProjectile();

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));

	//プレイヤーの移動制限を付ける
	const float moveLimit = 300.0f;
	float distance = Mathf::Length(transformComponent->worldTransform_.translation_);
	if (distance > moveLimit)
	{
		float scale = moveLimit / distance;
		transformComponent->worldTransform_.translation_ *= scale;
	}

	//Colliderを設定
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetDebugDrawEnabled(isDebug_);

	//ダメージエフェクトのスプライトの色を設定
	damagedSprite_->SetColor(damagedSpriteColor_);

	//HPの更新
	UpdateHP();

	//ImGuiの処理
	ImGui::Begin("Player");
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::DragFloat3("Velocity", &velocity.x);
	ImGui::DragFloat("AnimationTime", &animationTime_, 0.001f);
	for (int32_t i = 0; i < buttonSprites_.size(); ++i)
	{
		switch (i)
		{
		case A:
			ImGui::DragFloat2("AButtonPosition", &buttonSprites_[i].buttonPosition.x);
			ImGui::DragFloat2("AButtonScale", &buttonSprites_[i].buttonScale.x, 0.01f);
			ImGui::DragFloat2("AButtonFontPosition", &buttonSprites_[i].fontPosition.x);
			ImGui::DragFloat2("AButtonFontScale", &buttonSprites_[i].fontScale.x, 0.01f);
			break;
		case B:
			ImGui::DragFloat2("BButtonPosition", &buttonSprites_[i].buttonPosition.x);
			ImGui::DragFloat2("BButtonScale", &buttonSprites_[i].buttonScale.x, 0.01f);
			ImGui::DragFloat2("BButtonFontPosition", &buttonSprites_[i].fontPosition.x);
			ImGui::DragFloat2("BButtonFontScale", &buttonSprites_[i].fontScale.x, 0.01f);
			break;
		case X:
			ImGui::DragFloat2("XButtonPosition", &buttonSprites_[i].buttonPosition.x);
			ImGui::DragFloat2("XButtonScale", &buttonSprites_[i].buttonScale.x, 0.01f);
			ImGui::DragFloat2("XButtonFontPosition", &buttonSprites_[i].fontPosition.x);
			ImGui::DragFloat2("XButtonFontScale", &buttonSprites_[i].fontScale.x, 0.01f);
			break;
		case Y:
			ImGui::DragFloat2("YButtonPosition", &buttonSprites_[i].buttonPosition.x);
			ImGui::DragFloat2("YButtonScale", &buttonSprites_[i].buttonScale.x, 0.01f);
			ImGui::DragFloat2("YButtonFontPosition", &buttonSprites_[i].fontPosition.x);
			ImGui::DragFloat2("YButtonFontScale", &buttonSprites_[i].fontScale.x, 0.01f);
			break;
		case LB:
			ImGui::DragFloat2("LBButtonPosition", &buttonSprites_[i].buttonPosition.x);
			ImGui::DragFloat2("LBButtonScale", &buttonSprites_[i].buttonScale.x, 0.01f);
			ImGui::DragFloat2("LBButtonFontPosition", &buttonSprites_[i].fontPosition.x);
			ImGui::DragFloat2("LBButtonFontScale", &buttonSprites_[i].fontScale.x, 0.01f);
			break;
		case RB:
			ImGui::DragFloat2("RBButtonPosition", &buttonSprites_[i].buttonPosition.x);
			ImGui::DragFloat2("RBButtonScale", &buttonSprites_[i].buttonScale.x, 0.01f);
			ImGui::DragFloat2("RBButtonFontPosition", &buttonSprites_[i].fontPosition.x);
			ImGui::DragFloat2("RBButtonFontScale", &buttonSprites_[i].fontScale.x, 0.01f);
			break;
		}
		buttonSprites_[i].buttonSprite->SetPosition(buttonSprites_[i].buttonPosition);
		buttonSprites_[i].buttonSprite->SetScale(buttonSprites_[i].buttonScale);
		buttonSprites_[i].fontSprite->SetPosition(buttonSprites_[i].fontPosition);
		buttonSprites_[i].fontSprite->SetScale(buttonSprites_[i].fontScale);
	}
	if (ImGui::BeginCombo("AnimationName", currentAnimationName_.c_str()))
	{
		for (int i = 0; i < animationName_.size(); ++i)
		{
			const bool isSelected = (currentAnimationName_ == animationName_[i]);
			if (ImGui::Selectable(animationName_[i].c_str(), isSelected))
			{
				currentAnimationName_ = animationName_[i];
			}
			if (isSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
	ImGui::End();

	//デバッグ用の処理
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	if (isDebug_)
	{
		modelComponent->SetAnimationName(currentAnimationName_);
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(animationTime_);
	}

	//Gameobjectの更新
	GameObject::Update();

	//環境変数の適用
	ApplyGlobalVariables();
}

void Player::Draw(const Camera& camera)
{
	//Stateの描画
	state_->Draw(camera);

	//GameObjectの描画
	GameObject::Draw(camera);
}

void Player::DrawUI()
{
	//ダメージスプライトの描画
	damagedSprite_->Draw();

	//HPの描画
	hpSprite_->Draw();
	hpFrameSprite_->Draw();

	//ボタンのスプライトの描画
	for (int32_t i = 0; i < buttonSprites_.size(); ++i)
	{
		buttonSprites_[i].buttonSprite->Draw();
		buttonSprites_[i].fontSprite->Draw();
	}
}

void Player::OnCollision(GameObject* gameObject)
{
	//状態ごとの衝突判定
	state_->OnCollision(gameObject);

	//衝突相手が敵の場合
	if (Enemy* enemy = dynamic_cast<Enemy*>(gameObject))
	{
		//攻撃していなかった場合
		if (!enemy->GetIsAttack())
		{
			//押し戻し処理
			AABBCollider* AABB1 = gameObject->GetComponent<AABBCollider>();
			AABBCollider* AABB2 = GetComponent<AABBCollider>();
			Vector3 overlapAxis = {
				std::min<float>(AABB2->GetWorldCenter().x + AABB2->GetMax().x,AABB1->GetWorldCenter().x + AABB1->GetMax().x) -
				std::max<float>(AABB2->GetWorldCenter().x + AABB2->GetMin().x,AABB1->GetWorldCenter().x + AABB1->GetMin().x),
				std::min<float>(AABB2->GetWorldCenter().y + AABB2->GetMax().y,AABB1->GetWorldCenter().y + AABB1->GetMax().y) -
				std::max<float>(AABB2->GetWorldCenter().y + AABB2->GetMin().y,AABB1->GetWorldCenter().y + AABB1->GetMin().y),
				std::min<float>(AABB2->GetWorldCenter().z + AABB2->GetMax().z,AABB1->GetWorldCenter().z + AABB1->GetMax().z) -
				std::max<float>(AABB2->GetWorldCenter().z + AABB2->GetMin().z,AABB1->GetWorldCenter().z + AABB1->GetMin().z),
			};

			TransformComponent* transform1 = GetComponent<TransformComponent>();
			TransformComponent* transform2 = gameObject->GetComponent<TransformComponent>();
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
				directionAxis.z = (transform1->worldTransform_.translation_.z < transform2->worldTransform_.translation_.z) ? -1.0f : 1.0f;
				directionAxis.x = 0.0f;
				directionAxis.y = 0.0f;
			}
			transform1->worldTransform_.translation_ += overlapAxis * directionAxis;
		}
	}
}

void Player::OnCollisionEnter(GameObject* gameObject)
{
	//状態ごとの衝突判定
	state_->OnCollisionEnter(gameObject);
}

void Player::OnCollisionExit(GameObject* gameObject)
{
	//状態ごとの衝突判定
	state_->OnCollisionExit(gameObject);
}

void Player::ChangeState(IPlayerState* state)
{
	state->SetPlayer(this);
	state->Initialize();
	state_.reset(state);
}

void Player::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	rootParameters_.walkThreshold = globalVariables->GetFloatValue(groupName, "WalkThreshold");
	rootParameters_.walkSpeed = globalVariables->GetFloatValue(groupName, "WalkSpeed");
	rootParameters_.runThreshold = globalVariables->GetFloatValue(groupName, "RunThreshold");
	rootParameters_.runSpeed = globalVariables->GetFloatValue(groupName, "RunSpeed");
	dodgeParameters_.dodgeSpeed = globalVariables->GetFloatValue(groupName, "DodgeSpeed");
	jumpParameters_.firstSpeed = globalVariables->GetFloatValue(groupName, "JumpFirstSpeed");
	jumpParameters_.gravityAcceleration = globalVariables->GetFloatValue(groupName, "GravityAcceleration");
}

void Player::AddMagicProjectile(const MagicProjectile::MagicType magicType)
{
	//Modelを取得
	ModelComponent* modelComponent = GetComponent<ModelComponent>();

	//プレイヤーの左手のWorldTransformを取得
	WorldTransform worldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:LeftHand");

	//プレイヤーの左手のワールド座標を取得
	Vector3 leftHandWorldPosition = {
		worldTransform.matWorld_.m[3][0],
		worldTransform.matWorld_.m[3][1],
		worldTransform.matWorld_.m[3][2],
	};

	//敵のTransformを取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetHipWorldPosition();

	//速度ベクトル
	Vector3 velocity = { 0.0f,0.0f,1.0f };
	//敵の中心までのオフセット値
	const Vector3 enemyOffset = { 0.0f,5.0f,0.0f };
	//ロックオン中の場合
	if (lockOn_->ExistTarget())
	{
		//敵の方向へのベクトルを計算して速度を掛ける
		velocity = Mathf::Normalize(enemyPosition + enemyOffset - leftHandWorldPosition);
	}
	else
	{
		velocity = Mathf::RotateVector(velocity, destinationQuaternion_);
	}

	//強化魔法の場合
	float magicDamage = 0.0f;
	switch (magicType)
	{
	case MagicProjectile::MagicType::kNormal:
		velocity *= magicAttackParameters_.magicProjectileSpeed;
		magicDamage = magicAttackParameters_.damage;
		audio_->PlayAudio(fireAudioHandle_, false, 0.4f);
		break;
	case MagicProjectile::MagicType::kEnhanced:
		velocity *= magicAttackParameters_.enhancedMagicProjectileSpeed;
		magicDamage = magicAttackParameters_.enhancedMagicDamage;
		audio_->PlayAudio(fireAudioHandle_, false, 0.4f);
		break;
	case MagicProjectile::MagicType::kCharged:
		velocity *= magicAttackParameters_.magicProjectileSpeed;
		magicDamage = magicAttackParameters_.chargeMagicDamage;
		audio_->PlayAudio(chargeFireAudioHandle_, false, 0.4f);
		break;
	}

	//魔法弾を作成
	MagicProjectile* magicProjectile = GameObjectManager::GetInstance()->CreateGameObject<MagicProjectile>();
	magicProjectile->SetVelocity(velocity);
	magicProjectile->SetDamage(magicDamage);
	magicProjectile->SetMagicType(magicType);
	//Transformの追加
	TransformComponent* transformComponent = magicProjectile->AddComponent<TransformComponent>();
	transformComponent->Initialize();
	transformComponent->worldTransform_.quaternion_ = destinationQuaternion_;
	transformComponent->worldTransform_.translation_ = leftHandWorldPosition;
	transformComponent->worldTransform_.scale_ = magicAttackParameters_.magicProjectileScale;
	//ModelComponentの追加
	ModelComponent* magicProjectileModelComponent = magicProjectile->AddComponent<ModelComponent>();
	magicProjectileModelComponent->Initialize("Sphere", Transparent);
	magicProjectileModelComponent->GetModel()->GetMaterial(1)->SetEnableLighting(false);
	magicProjectileModelComponent->GetModel()->GetMaterial(1)->SetColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	magicProjectileModelComponent->SetTransformComponent(transformComponent);
	//Colliderの追加
	SphereCollider* collider = magicProjectile->AddComponent<SphereCollider>();
	collider->SetTransformComponent(transformComponent);
	collider->SetRadius(transformComponent->worldTransform_.scale_.x);
	collider->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Weapon"));
	collider->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Weapon"));
	//一度更新を入れる
	magicProjectile->Update();
}

void Player::UpdateChargeMagicProjectile()
{
	//Yボタンを離した時
	if (input_->IsPressButtonExit(XINPUT_GAMEPAD_Y))
	{
		//チャージが終了していたら
		if (chargeMagicAttackWork_.isChargeMagicComplete_)
		{
			//チャージのフラグをリセット
			chargeMagicAttackWork_.isChargeMagicComplete_ = false;

			//チャージ魔法攻撃のフラグを立てる
			chargeMagicAttackWork_.isChargeMagicAttack_ = true;

			//エミッターを削除
			if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("ChargeMagicFinished"))
			{
				emitter->SetIsDead(true);
			}
		}
	}

	//Yボタンを押しているとき
	if (input_->IsPressButton(XINPUT_GAMEPAD_Y))
	{
		//チャージが終了していない場合
		if (!chargeMagicAttackWork_.isChargeMagicComplete_)
		{
			//チャージタイマーを進める
			chargeMagicAttackWork_.chargeMagicTimer_ += GameTimer::GetDeltaTime();

			//チャージタイマーが一定値を超えた場合
			if (chargeMagicAttackWork_.chargeMagicTimer_ > magicAttackParameters_.chargeMagicHoldDuration)
			{
				//チャージが終了したフラグを立てる
				chargeMagicAttackWork_.isChargeMagicComplete_ = true;

				//タイマーをリセット
				chargeMagicAttackWork_.chargeMagicTimer_ = 0.0f;

				//左手にパーティクルを出す
				ParticleEmitter* emitter = EmitterBuilder()
					.SetEmitterName("ChargeMagicFinished")
					.SetRadius(0.2f)
					.SetEmitterLifeTime(100.0f)
					.SetCount(20)
					.SetFrequency(0.01f)
					.SetScale({ 0.1f,0.1f,0.1f }, { 0.14f,0.14f,0.14f })
					.SetColor({ 0.6f,0.0f,0.0f,1.0f }, { 1.0f,0.0f,0.0f,1.0f })
					.SetVelocity({ -0.02f,0.02f,-0.02f }, { 0.02f,0.06f,0.02f })
					.SetLifeTime(0.06f, 0.12f)
					.Build();
				particleSystem_->AddParticleEmitter(emitter);
			}
		}
	}
	//Yボタンを押していない場合タイマーをリセット
	else
	{
		chargeMagicAttackWork_.chargeMagicTimer_ = 0.0f;
	}

	//エミッターの存在する場合は座標を移動させる
	if (ParticleEmitter* emitter = particleSystem_->GetParticleEmitter("ChargeMagicFinished"))
	{
		//プレイヤーのモデル
		ModelComponent* modelComponent = GetComponent<ModelComponent>();

		//左手のWorldTransformを取得
		WorldTransform worldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:LeftHand");

		//プレイヤーの左手のワールド座標を取得
		Vector3 leftHandWorldPosition = {
			worldTransform.matWorld_.m[3][0],
			worldTransform.matWorld_.m[3][1],
			worldTransform.matWorld_.m[3][2],
		};

		//座標を設定する
		emitter->SetTranslate(leftHandWorldPosition);
	}
}

void Player::UpdateHP()
{
	//HPバーの処理
	hp_ = (hp_ <= 0.0f) ? 0.0f : hp_;
	Vector2 currentHPSize = { hpSpriteSize_.x * (hp_ / kMaxHP), hpSpriteSize_.y };
	hpSprite_->SetSize(currentHPSize);

	//プレイヤーの体力が一定量以下になったらVignetteをかける
	const float hpRatio = 4.0f;
	if (hp_ <= kMaxHP / hpRatio)
	{
		PostEffects::GetInstance()->GetVignette()->SetIsEnable(true);
	}
	else
	{
		PostEffects::GetInstance()->GetVignette()->SetIsEnable(false);
	}
}