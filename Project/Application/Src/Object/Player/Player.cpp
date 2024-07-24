#include "Player.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Collision/SphereCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void Player::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Inputのインスタンスを取得
	input_ = Input::GetInstance();

	//Stateの初期化
	ChangeState(new PlayerStateRoot());

	//パーティクルを生成
	particleSystem_ = ParticleManager::Create("ChargeMagicFinished");

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

	//チャージ魔法の更新
	UpdateChargeMagicProjectile();

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));

	//Colliderを設定
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetDebugDrawEnabled(isDebug_);

	//ImGuiの処理
	ImGui::Begin("Player");
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::DragFloat3("Velocity", &velocity.x);
	ImGui::DragFloat("AnimationTime", &animationTime_, 0.001f);
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
}

void Player::OnCollision(GameObject* gameObject)
{
	//状態ごとの衝突判定
	state_->OnCollision(gameObject);

	//衝突相手が敵の場合
	if (dynamic_cast<Enemy*>(gameObject))
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
			//Y軸方向で最小の重なりが発生している場合
			directionAxis.y = (transform1->worldTransform_.translation_.y < transform2->worldTransform_.translation_.y) ? -1.0f : 1.0f;
			directionAxis.x = 0.0f;
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
	TransformComponent* enemyTransformComponent = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetComponent<TransformComponent>();

	//速度ベクトル
	Vector3 velocity = { 0.0f,0.0f,1.0f };
	//敵の中心までのオフセット値
	const Vector3 enemyOffset = { 0.0f,5.0f,0.0f };
	//ロックオン中の場合
	if (lockOn_->ExistTarget())
	{
		//敵の方向へのベクトルを計算して速度を掛ける
		velocity = Mathf::Normalize(enemyTransformComponent->GetWorldPosition() + enemyOffset - leftHandWorldPosition);
	}
	else
	{
		velocity = Mathf::RotateVector(velocity, destinationQuaternion_);
	}

	//強化魔法の場合
	switch (magicType)
	{
	case MagicProjectile::MagicType::kNormal:
		velocity *= magicAttackParameters_.magicProjectileSpeed;
		break;
	case MagicProjectile::MagicType::kEnhanced:
		velocity *= magicAttackParameters_.enhancedMagicProjectileSpeed;
		break;
	case MagicProjectile::MagicType::kCharged:
		velocity *= magicAttackParameters_.magicProjectileSpeed;
		break;
	}

	//魔法弾を作成
	MagicProjectile* magicProjectile = GameObjectManager::GetInstance()->CreateGameObject<MagicProjectile>();
	magicProjectile->SetVelocity(velocity);
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