#include "Orb.h"
#include "Engine/3D/Model/ModelManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void Orb::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//トランスフォームの初期化
	InitializeTransform();

	//モデルの初期化
	InitializeModel();

	//パーティクルシステムの生成
	particleSystem_ = ParticleManager::Create("MagicProjectile");

	//音声データの読み込み
	fireAudioHandle_ = audio_->LoadAudioFile("Fire.mp3");
	chargeFireAudioHandle_ = audio_->LoadAudioFile("ChargeFire.mp3");
}

void Orb::Update()
{
	//追従対象の更新
	UpdateFollowTarget();

	//プレイヤー情報の更新
	UpdatePlayerInfo();

	//トランスフォームを更新
	UpdateTransform();

	//UVアニメーションの更新
	UpdateUVAnimation();

	//クールダウンのタイマーを進める
	if (magicAttackWork_.cooldownTimer > 0.0f)
	{
		magicAttackWork_.cooldownTimer -= GameTimer::GetDeltaTime();
	}

	//Yボタンが押されていた場合
	if (input_->IsPressButton(XINPUT_GAMEPAD_Y))
	{
		//魔法がチャージされていない場合チャージを始める
		if (!magicAttackWork_.isCharging)
		{
			magicAttackWork_.isCharging = true;
			magicAttackWork_.chargeTimer = 0.0f;
		}
		//魔法がチャージされている場合はチャージタイマーを進める
		else
		{
			magicAttackWork_.chargeTimer += GameTimer::GetDeltaTime();
		}
	}
	//Yボタンが押されていない場合
	else
	{
		//魔法がチャージ中の場合
		if (magicAttackWork_.isCharging)
		{
			//チャージの閾値を超えていた場合はチャージショットを生成
			if (magicAttackWork_.chargeTimer >= magicAttackParameters_.chargeTimeThreshold)
			{
				AddMagicProjectile(MagicProjectile::MagicType::kCharged);

				//パーティクルの生成
				CreateChargeMagicShotParticles();

				//チャージショットの音声データを再生
				audio_->PlayAudio(chargeFireAudioHandle_, false, 0.2f);
			}
			//チャージの閾値を超えていない場合
			else
			{
				//魔法攻撃のクールタイムを超えていた場合
				if (magicAttackWork_.cooldownTimer <= 0.0f)
				{
					//通常魔法を生成
					AddMagicProjectile(MagicProjectile::MagicType::kNormal);
					//クールダウンタイマーをリセット
					magicAttackWork_.cooldownTimer = magicAttackParameters_.cooldownTime;
					//通常魔法の音声データを再生
					audio_->PlayAudio(fireAudioHandle_, false, 0.2f);
				}
			}

			//フラグをリセット
			magicAttackWork_.isCharging = false;
			magicAttackWork_.chargeTimer = 0.0f;
		}
	}

	//基底クラスの更新
	GameObject::Update();
}

void Orb::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void Orb::DrawUI()
{
}

void Orb::OnCollision(GameObject* gameObject)
{
}

void Orb::OnCollisionEnter(GameObject* gameObject)
{
}

void Orb::OnCollisionExit(GameObject* gameObject)
{
}

void Orb::CreateChargeMagicShotParticles()
{
	//トランスフォームを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//パーティクルを出す
	for (uint32_t i = 0; i < 360; ++i)
	{
		//速度を決める
		const float kParticleVelocity = 0.3f;
		Quaternion rotate = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, i * (3.14f / 180.0f));
		Vector3 velocity = Mathf::RotateVector({ 0.0f,1.0f,0.0f }, destinationQuaternion_ * rotate) * kParticleVelocity;

		//エミッターの生成
		ParticleEmitter* emitter = EmitterBuilder()
			.SetEmitterName("ChargeMagic")
			.SetColor({ 1.0f, 0.4f, 0.4f, 1.0f }, { 1.0f, 0.4f, 0.4f, 1.0f })
			.SetEmitterLifeTime(0.1f)
			.SetCount(1)
			.SetFrequency(0.2f)
			.SetLifeTime(0.3f, 0.3f)
			.SetRadius(0.1f)
			.SetScale({ 0.1f,0.1f,0.1f }, { 0.3f,0.3f,0.3f })
			.SetTranslation(transformComponent->worldTransform_.translation_)
			.SetVelocity(velocity, velocity)
			.Build();
		particleSystem_->AddParticleEmitter(emitter);
	}
}

void Orb::InitializeTransform()
{
	//トランスフォームコンポーネントを取得し、スケールを設定
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.scale_ = { 0.4f, 0.4f, 0.4f };
}

void Orb::InitializeModel()
{
	//モデルコンポーネントを取得し、シャドウのキャストを無効化
	ModelComponent* orbModelComponent = AddComponent<ModelComponent>();
	Model* model = ModelManager::CreateFromModelFile("Orb", Opaque);
	model->SetCastShadows(false);
	orbModelComponent->SetModel(model);

	//マテリアルの初期化
	for (size_t i = 0; i < model->GetNumMaterials(); ++i)
	{
		Material* material = model->GetMaterial(i);
		material->SetColor(color_); //色の設定
		material->SetEnableLighting(false); //ライティングをしない
		material->SetEnvironmentCoefficient(0.0f); //環境係数の設定
	}
}

void Orb::UpdateFollowTarget()
{
	if (target_)
	{
		//追従対象のワールド座標を取得
		Vector3 targetPosition = {
			target_->matWorld_.m[3][0],
			target_->matWorld_.m[3][1],
			target_->matWorld_.m[3][2],
		};

		//現在の補間されたターゲット位置を更新
		interTarget_ = Mathf::Lerp(interTarget_, targetPosition, targetInterpolationSpeed_);
	}
}

void Orb::UpdatePlayerInfo()
{
	//プレイヤーの取得
	const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");

	//クォータニオンを補間
	destinationQuaternion_ = Mathf::Slerp(destinationQuaternion_, player->GetDestinationQuaternion(), quaternionInterpolationSpeed_);
}

void Orb::UpdateTransform()
{
	//追従対象からのオフセットを取得
	Vector3 offset = offset_;

	//オフセットベクトルをクォータニオンに基づいて回転させる
	offset = Mathf::RotateVector(offset, destinationQuaternion_);

	//トランスフォームコンポーネントを取得して位置を更新
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ = interTarget_ + offset;
}

void Orb::UpdateUVAnimation()
{
	//UVの位置をアニメーションさせる
	uvTranslation_.x += uvAnimationSpeed_ * GameTimer::GetDeltaTime();
	uvTranslation_.x = std::fmod(uvTranslation_.x, 1.0f);

	//モデルコンポーネントを取得してマテリアルのUVを更新
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	Model* model = modelComponent->GetModel();
	for (size_t i = 0; i < model->GetNumMaterials(); ++i)
	{
		Material* material = model->GetMaterial(i);
		material->SetColor(color_);
		material->SetUVTranslation(uvTranslation_);
	}
}

void Orb::AddMagicProjectile(MagicProjectile::MagicType type)
{
	//魔法の速度ベクトルの計算
	Vector3 velocity = CalculateMagicProjectileVelocity();

	//ノックバックの設定
	KnockbackSettings knockbackSettings{};
	if (type == MagicProjectile::MagicType::kCharged)
	{
		//プレイヤーの取得
		const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");

		//ノックバック速度を設定
		Vector3 knockbackVelocity = Mathf::RotateVector({ 0.0f, 0.0f, magicAttackParameters_.chargeMagicKnockbackVelocity.z }, player->GetDestinationQuaternion());
		knockbackVelocity.y = magicAttackParameters_.chargeMagicKnockbackVelocity.y;
		knockbackSettings.knockbackVelocity = knockbackVelocity;

		//ノックバック加速度を設定
		Vector3 knockbackAcceleration = Mathf::RotateVector({ 0.0f, 0.0f, magicAttackParameters_.chargeMagicKnockbackAcceleration.z }, player->GetDestinationQuaternion());
		knockbackAcceleration.y = magicAttackParameters_.chargeMagicKnockbackAcceleration.y;
		knockbackSettings.knockbackAcceleration = knockbackAcceleration;

		//ノックバックの時間を設定
		knockbackSettings.knockbackDuration = magicAttackParameters_.chargeMagicKnockbackDuration;

		//リアクションタイプを設定
		knockbackSettings.reactionType_ = ReactionType::Knockback;
	}

	//ダメージを設定
	float damage = (type == MagicProjectile::MagicType::kCharged) ? magicAttackParameters_.chargeMagicDamage : magicAttackParameters_.normalMagicDamage;

	//魔法を生成
	MagicProjectile* magicProjectile = GameObjectManager::CreateGameObject<MagicProjectile>();
	magicProjectile->SetMagicType(type);
	magicProjectile->SetVelocity(velocity);
	magicProjectile->SetKnockbackSettings(knockbackSettings);
	magicProjectile->SetDamage(damage);

	//トランスフォームの初期化
	InitializeMagicProjectileTransform(magicProjectile);
}

Vector3 Orb::CalculateMagicProjectileVelocity()
{
	//速度ベクトル
	Vector3 velocity = { 0.0f, 0.0f, magicAttackParameters_.magicProjectileSpeed };
	//ロックオン中の場合
	if (lockon_->ExistTarget())
	{
		//オーブのトランスフォームを取得
		Vector3 orbPosition = GetComponent<TransformComponent>()->worldTransform_.translation_;
		//敵の座標を取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("")->GetHipWorldPosition();
		//速度を計算
		velocity = Mathf::Normalize(enemyPosition - orbPosition) * magicAttackParameters_.magicProjectileSpeed;
	}
	else
	{
		//プレイヤーの取得
		const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");
		//プレイヤーのクォータニオンに基づいて速度を回転
		velocity = Mathf::RotateVector(velocity, player->GetDestinationQuaternion());
	}
	return velocity;
}

void Orb::InitializeMagicProjectileTransform(MagicProjectile* magicProjectile)
{
	//プレイヤーの取得
	const Player* player = GameObjectManager::GetInstance()->GetConstGameObject<Player>("");

	//魔法のトランスフォームをオーブの位置に初期化
	TransformComponent* orbTransformComponent = GetComponent<TransformComponent>();
	TransformComponent* transformComponent = magicProjectile->GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ = orbTransformComponent->GetWorldPosition();
	transformComponent->worldTransform_.quaternion_ = player->GetDestinationQuaternion();
	transformComponent->Update();
}

//void Orb::InitializeMagicProjectileModel(MagicProjectile* magicProjectile)
//{
//	//魔法のモデルコンポーネントを初期化
//	ModelComponent* modelComponent = magicProjectile->AddComponent<ModelComponent>();
//	modelComponent->SetModel(ModelManager::CreateFromModelFile("Sphere", Opaque));
//}
//
//void Orb::InitializeMagicProjectileCollider(MagicProjectile* magicProjectile)
//{
//	//魔法のコライダーを初期化
//	SphereCollider* sphereCollider = magicProjectile->AddComponent<SphereCollider>();
//	sphereCollider->SetCollisionEnabled(false);
//}