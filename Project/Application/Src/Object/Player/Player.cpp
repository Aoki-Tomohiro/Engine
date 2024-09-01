#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Player/States/PlayerStateDeath.h"

void Player::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//スキルクールダウンマネージャーの生成
	skillCooldownManager_ = std::make_unique<SkillCooldownManager>();
	skillCooldownManager_->AddSkill(Skill::kLaunchAttack, &launchAttackParameters_);
	skillCooldownManager_->AddSkill(Skill::kSpinAttack, &spinAttackParameters_);

	//パーティクルシステムの初期化
	InitializeParticleSystems();

	//状態の初期化
	ChangeState(new PlayerStateRoot());

	//トランスフォームコンポーネントを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	//回転の種類をQuaternionに設定
	transformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
	//Quaternionの初期化
	destinationQuaternion_ = transformComponent->worldTransform_.quaternion_;

	//モデルコンポーネントを取得
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	//全てのマテリアルの環境マップの映り込みの係数を設定
	for (size_t i = 0; i < modelComponent->GetModel()->GetNumMaterials(); ++i)
	{
		modelComponent->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
	}

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
		case RT:
			TextureManager::Load("xbox_rt_outline.png");
			TextureManager::Load("Change.png");
			buttonSprites_[RT].buttonPosition = { 1070.0f,370.0f };
			buttonSprites_[RT].buttonScale = { 0.5f,0.5f };
			buttonSprites_[RT].buttonSprite.reset(Sprite::Create("xbox_rt_outline.png", buttonSprites_[RT].buttonPosition));
			buttonSprites_[RT].buttonSprite->SetScale(buttonSprites_[RT].buttonScale);
			buttonSprites_[RT].fontPosition = { 1139.0f,382.0f };
			buttonSprites_[RT].fontScale = { 0.3f,0.3f };
			buttonSprites_[RT].fontSprite.reset(Sprite::Create("Change.png", buttonSprites_[RT].fontPosition));
			buttonSprites_[RT].fontSprite->SetScale(buttonSprites_[RT].fontScale);
			break;
		}
	}

	//ボタンのスプライトの生成
	RTbuttonSprites_.resize(kMaxButtons);
	for (int32_t i = 0; i < kMaxButtons; i++)
	{
		switch (i)
		{
		case A:
			TextureManager::Load("xbox_button_a_outline.png");
			TextureManager::Load("Jump.png");
			RTbuttonSprites_[A].buttonPosition = { 1000.0f,630.0f };
			RTbuttonSprites_[A].buttonSprite.reset(Sprite::Create("xbox_button_a_outline.png", RTbuttonSprites_[A].buttonPosition));
			RTbuttonSprites_[A].fontPosition = { 1060.0f,644.0f };
			RTbuttonSprites_[A].fontScale = { 0.3f,0.3f };
			RTbuttonSprites_[A].fontSprite.reset(Sprite::Create("Jump.png", RTbuttonSprites_[A].fontPosition));
			RTbuttonSprites_[A].fontSprite->SetScale(RTbuttonSprites_[A].fontScale);
			break;
		case B:
			TextureManager::Load("xbox_button_b_outline.png");
			TextureManager::Load("Dash.png");
			RTbuttonSprites_[B].buttonPosition = { 1048.0f,582.0f };
			RTbuttonSprites_[B].buttonSprite.reset(Sprite::Create("xbox_button_b_outline.png", RTbuttonSprites_[B].buttonPosition));
			RTbuttonSprites_[B].fontPosition = { 1108.0f,596.0f };
			RTbuttonSprites_[B].fontScale = { 0.3f,0.3f };
			RTbuttonSprites_[B].fontSprite.reset(Sprite::Create("Dash.png", RTbuttonSprites_[B].fontPosition));
			RTbuttonSprites_[B].fontSprite->SetScale(RTbuttonSprites_[B].fontScale);
			break;
		case X:
			TextureManager::Load("xbox_button_x_outline.png");
			TextureManager::Load("LaunchAttack.png");
			RTbuttonSprites_[X].buttonPosition = { 952.0f, 582.0f };
			RTbuttonSprites_[X].buttonSprite.reset(Sprite::Create("xbox_button_x_outline.png", RTbuttonSprites_[X].buttonPosition));
			RTbuttonSprites_[X].fontPosition = { 790.0f,596.0f };
			RTbuttonSprites_[X].fontScale = { 0.3f,0.3f };
			RTbuttonSprites_[X].fontSprite.reset(Sprite::Create("LaunchAttack.png", RTbuttonSprites_[X].fontPosition));
			RTbuttonSprites_[X].fontSprite->SetScale(RTbuttonSprites_[X].fontScale);
			break;
		case Y:
			TextureManager::Load("xbox_button_y_outline.png");
			TextureManager::Load("SpinAttack.png");
			RTbuttonSprites_[Y].buttonPosition = { 1000.0f,534.0f };
			RTbuttonSprites_[Y].buttonSprite.reset(Sprite::Create("xbox_button_y_outline.png", RTbuttonSprites_[Y].buttonPosition));
			RTbuttonSprites_[Y].fontPosition = { 880.0f,544.0f };
			RTbuttonSprites_[Y].fontScale = { 0.3f,0.3f };
			RTbuttonSprites_[Y].fontSprite.reset(Sprite::Create("SpinAttack.png", RTbuttonSprites_[Y].fontPosition));
			RTbuttonSprites_[Y].fontSprite->SetScale(RTbuttonSprites_[Y].fontScale);
			break;
		case LB:
			TextureManager::Load("xbox_lb_outline.png");
			TextureManager::Load("Lockon.png");
			RTbuttonSprites_[LB].buttonPosition = { 1070.0f,429.0f };
			RTbuttonSprites_[LB].buttonSprite.reset(Sprite::Create("xbox_lb_outline.png", RTbuttonSprites_[LB].buttonPosition));
			RTbuttonSprites_[LB].fontPosition = { 1139.0f,439.0f };
			RTbuttonSprites_[LB].fontScale = { 0.3f,0.3f };
			RTbuttonSprites_[LB].fontSprite.reset(Sprite::Create("Lockon.png", RTbuttonSprites_[LB].fontPosition));
			RTbuttonSprites_[LB].fontSprite->SetScale(RTbuttonSprites_[LB].fontScale);
			break;
		case RB:
			TextureManager::Load("xbox_rb_outline.png");
			TextureManager::Load("Dodge.png");
			RTbuttonSprites_[RB].buttonPosition = { 1070.0f,484.0f };
			RTbuttonSprites_[RB].buttonSprite.reset(Sprite::Create("xbox_rb_outline.png", RTbuttonSprites_[RB].buttonPosition));
			RTbuttonSprites_[RB].fontPosition = { 1139.0f,496.0f };
			RTbuttonSprites_[RB].fontScale = { 0.3f,0.3f };
			RTbuttonSprites_[RB].fontSprite.reset(Sprite::Create("Dodge.png", RTbuttonSprites_[RB].fontPosition));
			RTbuttonSprites_[RB].fontSprite->SetScale(RTbuttonSprites_[RB].fontScale);
			break;
		case RT:
			TextureManager::Load("xbox_rt_outline.png");
			TextureManager::Load("Change.png");
			RTbuttonSprites_[RT].buttonPosition = { 1070.0f,370.0f };
			RTbuttonSprites_[RT].buttonScale = { 0.5f,0.5f };
			RTbuttonSprites_[RT].buttonSprite.reset(Sprite::Create("xbox_rt_outline.png", RTbuttonSprites_[RT].buttonPosition));
			RTbuttonSprites_[RT].buttonSprite->SetScale(RTbuttonSprites_[RT].buttonScale);
			RTbuttonSprites_[RT].fontPosition = { 1139.0f,382.0f };
			RTbuttonSprites_[RT].fontScale = { 0.3f,0.3f };
			RTbuttonSprites_[RT].fontSprite.reset(Sprite::Create("Change.png", RTbuttonSprites_[RT].fontPosition));
			RTbuttonSprites_[RT].fontSprite->SetScale(RTbuttonSprites_[RT].fontScale);
			break;
		}
	}

	skillCoolDownSprites_.resize(2);
	skillCoolDownSprites_[0].reset(Sprite::Create("white.png", { 955.0f, 580.0f }));
	skillCoolDownSprites_[1].reset(Sprite::Create("white.png", { 1004.0f,530.0f }));

	//音声データの読み込み
	damageAudioHandle_ = audio_->LoadAudioFile("Damage.mp3");

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "ColliderOffset", colliderOffset_);
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

	//スキルクールダウンマネージャーの更新
	skillCooldownManager_->Update();

	//スキルのクールダウンのスプライトのサイズを設定
	float currentScale = kMaxCoolDownSpriteScale.x * (skillCooldownManager_->GetCooldownTime(Skill::kLaunchAttack) / launchAttackParameters_.cooldownDuration);
	skillCoolDownSprites_[0]->SetScale({ currentScale , kMaxCoolDownSpriteScale.y });
	currentScale = kMaxCoolDownSpriteScale.x * (skillCooldownManager_->GetCooldownTime(Skill::kSpinAttack) / spinAttackParameters_.cooldownDuration);
	skillCoolDownSprites_[1]->SetScale({ currentScale , kMaxCoolDownSpriteScale.y });

	//ダメージのスプライトの色を設定
	damagedSprite_->SetColor(damagedSpriteColor_);

	//回転処理
	UpdateRotation();

	//コライダーの更新
	UpdateCollider();

	//移動制限の処理
	RestrictPlayerMovement(100.0f);

	//HPの更新
	UpdateHP();

	//死亡状態かどうかを確認する
	CheckAndTransitionToDeath();

	//デバッグ時の更新処理
	DebugUpdate();

	//プレイヤーの移動制限を付ける
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	const float moveLimit = 300.0f;
	float distance = Mathf::Length(transformComponent->worldTransform_.translation_);
	if (distance > moveLimit)
	{
		float scale = moveLimit / distance;
		transformComponent->worldTransform_.translation_ *= scale;
	}

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
	const float threshold = 0.7f;

	//ダメージスプライトの描画
	damagedSprite_->Draw();

	//HPの描画
	hpSprite_->Draw();
	hpFrameSprite_->Draw();

	//RTを押しているとき
	if (input_->GetRightTriggerValue() > threshold)
	{
		//ボタンのスプライトの描画
		for (int32_t i = 0; i < RTbuttonSprites_.size(); ++i)
		{
			RTbuttonSprites_[i].buttonSprite->Draw();
			RTbuttonSprites_[i].fontSprite->Draw();
		}
	}
	else
	{
		//ボタンのスプライトの描画
		for (int32_t i = 0; i < buttonSprites_.size(); ++i)
		{
			buttonSprites_[i].buttonSprite->Draw();
			buttonSprites_[i].fontSprite->Draw();
		}
	}

	for (int32_t i = 0; i < 2; i++)
	{
		skillCoolDownSprites_[i]->Draw();
	}
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
		AABBCollider* AABB2 = GetComponent<AABBCollider>();

		//2つのAABBコライダー間のオーバーラップ軸を計算
		Vector3 overlapAxis = {
			std::min<float>(AABB2->GetWorldCenter().x + AABB2->GetMax().x, AABB1->GetWorldCenter().x + AABB1->GetMax().x) -
			std::max<float>(AABB2->GetWorldCenter().x + AABB2->GetMin().x, AABB1->GetWorldCenter().x + AABB1->GetMin().x),
			std::min<float>(AABB2->GetWorldCenter().y + AABB2->GetMax().y, AABB1->GetWorldCenter().y + AABB1->GetMax().y) -
			std::max<float>(AABB2->GetWorldCenter().y + AABB2->GetMin().y, AABB1->GetWorldCenter().y + AABB1->GetMin().y),
			std::min<float>(AABB2->GetWorldCenter().z + AABB2->GetMax().z, AABB1->GetWorldCenter().z + AABB1->GetMax().z) -
			std::max<float>(AABB2->GetWorldCenter().z + AABB2->GetMin().z, AABB1->GetWorldCenter().z + AABB1->GetMin().z),
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
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ += velocity * GameTimer::GetDeltaTime();
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

void Player::PlayDamageSound()
{
	//ダメージの音を再生
	audio_->PlayAudio(damageAudioHandle_, false, 0.2f);
}

void Player::PlayAnimation(const std::string& name, const float speed, const bool loop)
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->PlayAnimation(name, speed, loop);
}

void Player::StopAnimation()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->StopAnimation();
}

void Player::PauseAnimation()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->PauseAnimation();
}

void Player::ResumeAnimation()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->ResumeAnimation();
}

void Player::SetIsAnimationBlending(const bool isBlending)
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->SetIsBlending(isBlending);
}

void Player::SetAnimationSpeed(const float animationSpeed)
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->SetCurrentAnimationSpeed(animationSpeed);
}

float Player::GetCurrentAnimationTime()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetCurrentAnimationTime();
}

void Player::SetCurrentAnimationTime(const float animationTime)
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->SetCurrentAnimationTime(animationTime);
}

float Player::GetNextAnimationTime()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetNextAnimationTime();
}

void Player::SetNextAnimationTime(const float animationTime)
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->SetNextAnimationTime(animationTime);
}

bool Player::GetIsAnimationFinished()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetIsAnimationFinished();
}

bool Player::GetIsBlendingCompleted()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetIsBlendingCompleted();
}

float Player::GetAnimationDuration()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetCurrentAnimationDuration();
}

float Player::GetNextAnimationDuration()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetNextAnimationDuration();
}

Vector3 Player::GetHipLocalPosition()
{
	//トランスフォームコンポーネントを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//腰のジョイントのローカル座標を計算
	Vector3 hipLocalPosition = GetHipWorldPosition() - transformComponent->GetWorldPosition();

	//腰のジョイントのローカル座標を返す
	return hipLocalPosition;
}

Vector3 Player::GetHipWorldPosition()
{
	//モデルコンポーネントを取得
	ModelComponent* modelComponent = GetComponent<ModelComponent>();

	//腰のジョイントのワールドトランスフォームを取得
	WorldTransform hipWorldTransform = modelComponent->GetModel()->GetJointWorldTransform("mixamorig:Hips");

	//ワールド座標を取得しVector3に変換して返す
	return Vector3{
		hipWorldTransform.matWorld_.m[3][0],
		hipWorldTransform.matWorld_.m[3][1],
		hipWorldTransform.matWorld_.m[3][2],
	};
}

Vector3 Player::GetPosition()
{
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	return transformComponent->worldTransform_.translation_;
}

void Player::SetPosition(const Vector3& position)
{
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ = position;
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

void Player::InitializeParticleSystems()
{
	//テクスチャの読み込み
	TextureManager::Load("smoke_01.png");

	//パーティクルシステムの生成
	particleSystems_["Dash"] = ParticleManager::Create("Dash");
	particleSystems_["Dash"]->SetTexture("smoke_01.png");
}

void Player::UpdateRotation()
{
	//トランスフォームコンポーネントを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//現在のクォータニオンと目的のクォータニオンを補間
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));
}

void Player::UpdateCollider()
{
	//AABBColliderを取得
	AABBCollider* collider = GetComponent<AABBCollider>();

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
	collider->SetCenter(hipPosition + colliderOffset_);
}

void Player::RestrictPlayerMovement(float moveLimit)
{
	//TransformComponentを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//プレイヤーの現在位置から原点までの距離を計算
	float distance = Mathf::Length(transformComponent->worldTransform_.translation_);

	//距離が移動制限を超えているかどうかを確認
	if (distance > moveLimit) {
		//スケールを計算して移動制限範囲に収めるよう位置を調整
		float scale = moveLimit / distance;
		transformComponent->worldTransform_.translation_ *= scale;
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
	//アニメーターコンポーネントを取得
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();

	//デバッグ状態の場合
	if (isDebug_)
	{
		//アニメーションの時間を設定
		animatorComponent->SetAnimationTime(currentAnimationName_, animationTime_);

		//アニメーションのブレンドを無効化する
		animatorComponent->SetIsBlending(false);
	}
	else
	{
		//アニメーションのブレンドを有効化する
		animatorComponent->SetIsBlending(true);
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
	//アニメーターコンポーネントを取得
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();

	//ImGuiのウィンドウを開始
	ImGui::Begin("Player");

	//デバッグモードのチェックボックス
	ImGui::Checkbox("IsDebug", &isDebug_);

	//デバッグ状態の場合
	if (isDebug_)
	{
		//アニメーション時間のスライダー
		ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f, 0.0f, animatorComponent->GetAnimation(currentAnimationName_)->GetDuration());

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
					animatorComponent->PlayAnimation(currentAnimationName_, 1.0f, true);
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