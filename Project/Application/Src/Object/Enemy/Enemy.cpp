#include "Enemy.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/SphereCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/MagicProjectile/MagicProjectile.h"
#include "Application/Src/Object/Enemy/States/EnemyStateRoot.h"

void Enemy::Initialize()
{
	//Audioのインスタンスを取得
	audio_ = Audio::GetInstance();

	//GameObjectの初期化
	GameObject::Initialize();

	//Stateの初期化
	ChangeState(new EnemyStateRoot());

	//パーティクルを生成
	particleSystem_ = ParticleManager::Create("Enemy");

	//体力のスプライトの生成
	TextureManager::Load("HpBar.png");
	hpSprite_.reset(Sprite::Create("HpBar.png", hpSpritePosition_));
	hpSprite_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });
	TextureManager::Load("HpBarFrame.png");
	hpFrameSprite_.reset(Sprite::Create("HpBarFrame.png", hpFrameSpritePosition_));
	hpFrameSprite_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });

	//音声データの読み込み
	tackleAudioHandle_ = audio_->LoadAudioFile("Tackle.mp3");
	comboAudioHandle1_ = audio_->LoadAudioFile("EnemyComboAttack1.mp3");
	comboAudioHandle2_ = audio_->LoadAudioFile("EnemyComboAttack2.mp3");
	comboAudioHandle3_ = audio_->LoadAudioFile("EnemyComboAttack3.mp3");
	jumpAttackAudioHandle_ = audio_->LoadAudioFile("JumpAttack.mp3");
	chargeAudioHandle_ = audio_->LoadAudioFile("Charge.mp3");
	laserAudioHandle_ = audio_->LoadAudioFile("Laser.mp3");
}

void Enemy::Update()
{
	//アニメーションの速度の更新
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(timeScale_);

	//動いていない状態の時
	if (!isMove_)
	{
		//プレイヤーを取得
		if (Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>())
		{	
			//プレイヤーの速度が0でなければ
			if (player->GetVelocity() != Vector3{ 0.0f,0.0f,0.0f })
			{
				//移動フラグを立てる
				isMove_ = true;

				//アニメーションを歩きにする
				ModelComponent* modelComponent = GetComponent<ModelComponent>();
				modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0");
			}
		}
	}

	//タイトルシーンにいる場合は移動処理をスキップ
	if (!isInTitleScene_)
	{
		//ImGui
		if (isMove_)
		{
			ImGui::Begin("Enemy");
			ImGui::Checkbox("IsDebug", &isDebug_);
			ImGui::Checkbox("IsAnimationPlaying", &isAnimationStop_);
			ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f);
			ImGui::End();
		}
		//Stateの更新
		state_->Update();
	}

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));

	//Colliderを設定
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetDebugDrawEnabled(isDebug_);

	//HPの更新
	UpdateHP();

	//GameObjectの更新
	GameObject::Update();
}

void Enemy::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Enemy::DrawUI()
{
	//HPの描画
	hpSprite_->Draw();
	hpFrameSprite_->Draw();
}

void Enemy::OnCollision(GameObject* gameObject)
{
	//ダメージ倍率
	float damageRate = 1.0f;
	//パリィ中の場合は倍率を上げる
	if (isParrying_)
	{
		const float kParryingDamageRate = 1.2f;
		damageRate = kParryingDamageRate;
	}

	//衝突相手がプレイヤーだった場合
	if (Player* player = dynamic_cast<Player*>(gameObject))
	{
		if (!player->GetIsKnockback())
		{
			state_->SetIsAttack(false);
		}
	}
	//衝突相手が武器だった場合
	else if (Weapon* weapon = dynamic_cast<Weapon*>(gameObject))
	{
		Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
		hp_ -= player->GetDamage() * damageRate;
	}
	//衝突相手が魔法だった場合
	else if (MagicProjectile* magicProjectile = dynamic_cast<MagicProjectile*>(gameObject))
	{
		hp_ -= magicProjectile->GetDamage() * damageRate;
	}
}

void Enemy::OnCollisionEnter(GameObject* gameObject)
{

}

void Enemy::OnCollisionExit(GameObject* gameObject)
{

}

const Vector3 Enemy::GetHipWorldPosition()
{
	//腰のJointの座標を取得
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	WorldTransform hipWorldTransform = modelComponent->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips");
	Vector3 hipWorldPosition = {
		hipWorldTransform.matWorld_.m[3][0],
		hipWorldTransform.matWorld_.m[3][1],
		hipWorldTransform.matWorld_.m[3][2],
	};
	return hipWorldPosition;
}

const Vector3 Enemy::GetHipLocalPosition()
{
	//ローカルの腰の座標を取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	Vector3 hipLocalPosition = GetHipWorldPosition() - transformComponent->GetWorldPosition();
	return hipLocalPosition;
}

void Enemy::ChangeState(IEnemyState* state)
{
	state->SetEnemy(this);
	state->Initialize();
	state_.reset(state);
}

void Enemy::CreateWarningParticle()
{
	//パーティクルを出す
	for (uint32_t i = 0; i < 360; ++i)
	{
		//速度を決める
		TransformComponent* transformComponent = GetComponent<TransformComponent>();
		const float kParticleVelocity = 0.1f;
		const float distance = 1.2f;
		Quaternion rotate = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,0.0f,1.0f }, i * (3.14f / 180.0f));
		Vector3 direction = Mathf::RotateVector({ 0.0f,1.0f,0.0f }, destinationQuaternion_ * rotate);

		//エミッターの生成
		ParticleEmitter* emitter = EmitterBuilder()
			.SetEmitterName("ChargeUp")
			.SetColor({ 0.6f,0.5f,0.0f,1.0f }, { 1.0f,0.5f,0.0f,1.0f })
			.SetEmitterLifeTime(0.1f)
			.SetCount(1)
			.SetFrequency(0.2f)
			.SetLifeTime(0.4f, 0.4f)
			.SetRadius(0.1f)
			.SetScale({ 0.1f,0.1f,0.1f }, { 0.3f,0.3f,0.3f })
			.SetTranslation(GetHipWorldPosition() + direction * distance)
			.SetVelocity(direction * kParticleVelocity, direction * kParticleVelocity)
			.Build();
		particleSystem_->AddParticleEmitter(emitter);
	}
}

void Enemy::UpdateHP()
{
	//HPバーの処理
	hp_ = (hp_ <= 0.0f) ? 0.0f : hp_;
	Vector2 currentHPSize = { hpSpriteSize_.x * (hp_ / kMaxHP), hpSpriteSize_.y };
	hpSprite_->SetSize(currentHPSize);
}

Warning* Enemy::CreateBoxWarningObject(const Warning::BoxWarningObjectSettings& warningObjectSettings)
{
	//警告用のオブジェクトを生成
	Warning* warning = GameObjectManager::GetInstance()->CreateGameObject<Warning>();
	warning->SetBoxWarningObjectSettings(warningObjectSettings);

	//Transformの追加
	TransformComponent* warningTransformComponent = warning->AddComponent<TransformComponent>();
	warningTransformComponent->Initialize();
	warningTransformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;

	//Modelの追加
	ModelComponent* warningModelComponent = warning->AddComponent<ModelComponent>();
	switch (warningObjectSettings.warningPrimitive)
	{
	case Warning::kBox:
		warningModelComponent->Initialize("Cube", Transparent);
		break;
	case Warning::kSphere:
		warningModelComponent->Initialize("Sphere", Transparent);
		break;
	}
	warningModelComponent->SetTransformComponent(warningTransformComponent);
	warningModelComponent->GetModel()->GetMaterial(1)->SetColor({ 1.0f,0.0f,0.0f,0.6f });

	//Colliderの追加
	Collider* collider = nullptr;
	switch (warningObjectSettings.warningPrimitive)
	{
	case Warning::kBox:
		collider = warning->AddComponent<OBBCollider>();
		break;
	case Warning::kSphere:
		collider = warning->AddComponent<SphereCollider>();
		break;
	}
	collider->Initialize();
	collider->SetTransformComponent(warningTransformComponent);
	collider->SetCollisionAttribute(CollisionAttributeManager::GetInstance()->GetAttribute("Warning"));
	collider->SetCollisionMask(CollisionAttributeManager::GetInstance()->GetMask("Warning"));

	return warning;
}