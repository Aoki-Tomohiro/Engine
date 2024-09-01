#include "Weapon.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void Weapon::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//コライダーの情報をもとに武器の名前を設定
	UpdateObjectNameFromCollider();

	//パーティクルシステムの作成
	particleSystem_ = ParticleManager::Create(name_);

	//加速フィールドの生成
	CreateAccelerationField();

	//環境変数の初期化
	InitializeGlobalVariables();

	//音声データの読み込み
	hitAudioHandle_ = audio_->LoadAudioFile("Hit.mp3");
}

void Weapon::Update()
{
	//ヒットフラグのリセット
	isHit_ = false;

	//コライダーの更新
	if (name_ == "PlayerWeapon")
	{
		UpdatePlayerCollider();
	}
	else
	{
		UpdateEnemyCollider();
	}

	//基底クラスの更新
	GameObject::Update();

	//環境変数の適用
	ApplyGlobalVariables();

	//ImGuiの更新
	UpdateImGui();
}

void Weapon::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void Weapon::DrawUI()
{
}

void Weapon::OnCollision(GameObject* gameObject)
{
	//ヒットフラグを立てる
	isHit_ = true;

	//プレイヤーの武器の場合
	if (name_ == "PlayerWeapon")
	{
		//ヒットストップを有効化する
		hitStop_->Start(effectSettings_.hitStopDuration);

		//音声を再生
		audio_->PlayAudio(hitAudioHandle_, false, 0.2f);

		//パーティクルを出す
		OBBCollider* collider = GetComponent<OBBCollider>();
		ParticleEmitter* newEmitter = new ParticleEmitter();
		newEmitter->Initialize("Hit", 1.0f);
		newEmitter->SetTranslate(collider->GetWorldCenter());
		newEmitter->SetCount(200);
		newEmitter->SetColorMin({ 1.0f, 0.2f, 0.2f, 1.0f });
		newEmitter->SetColorMax({ 1.0f, 0.2f, 0.2f, 1.0f });
		newEmitter->SetFrequency(2.0f);
		newEmitter->SetLifeTimeMin(0.2f);
		newEmitter->SetLifeTimeMax(0.4f);
		newEmitter->SetRadius(0.0f);
		newEmitter->SetScaleMin({ 0.2f,0.2f,0.2f });
		newEmitter->SetScaleMax({ 0.3f,0.3f,0.3f });
		newEmitter->SetVelocityMin({ -0.6f,-0.6f,-0.6f });
		newEmitter->SetVelocityMax({ 0.6f,0.6f,0.6f });
		particleSystem_->AddParticleEmitter(newEmitter);
	}
}

void Weapon::OnCollisionEnter(GameObject* gameObject)
{
}

void Weapon::OnCollisionExit(GameObject* gameObject)
{
}

void Weapon::SetHitbox(const Hitbox& hitbox)
{
	if ((Mathf::Length(hitbox.center) < 0.1f) && (Mathf::Length(hitbox.size) < 0.1f))
	{
		hitbox_.center = defaultColliderOffset_;
		hitbox_.size = defaultColliderSize_;
	}
	else
	{
		hitbox_.center = hitbox.center;
		hitbox_.size = hitbox.size;
	}
}

void Weapon::UpdatePlayerCollider()
{
	//プレイヤーを取得
	Player* player = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("");

	//プレイヤーのコライダーを取得
	AABBCollider* playerCollider = player->GetComponent<AABBCollider>();

	//コライダーを取得
	OBBCollider* collider = GetComponent<OBBCollider>();

	//攻撃中の場合はコライダーを有効にする
	collider->SetCollisionEnabled(isAttack_);

	//中心座標を設定
	Vector3 center = Mathf::RotateVector(hitbox_.center, player->GetDestinationQuaternion());
	collider->SetWorldCenter(player->GetHipWorldPosition() + center);

	//プレイヤーのトランスフォームを取得
	TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();

	//各軸を設定
	collider->SetOrientations(
		{ playerTransformComponent->worldTransform_.matWorld_.m[0][0], playerTransformComponent->worldTransform_.matWorld_.m[0][1], playerTransformComponent->worldTransform_.matWorld_.m[0][2] },
		{ playerTransformComponent->worldTransform_.matWorld_.m[1][0], playerTransformComponent->worldTransform_.matWorld_.m[1][1], playerTransformComponent->worldTransform_.matWorld_.m[1][2] },
		{ playerTransformComponent->worldTransform_.matWorld_.m[2][0], playerTransformComponent->worldTransform_.matWorld_.m[2][1], playerTransformComponent->worldTransform_.matWorld_.m[2][2] }
	);

	//サイズを設定
	collider->SetSize(hitbox_.size);

	//デバッグ表示の設定
	collider->SetDebugDrawEnabled(isDebug_);
}

void Weapon::UpdateEnemyCollider()
{
	//敵を取得
	Enemy* enemy = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("");

	//敵のコライダーを取得
	AABBCollider* enemyCollider = enemy->GetComponent<AABBCollider>();

	//コライダーを取得
	OBBCollider* collider = GetComponent<OBBCollider>();

	//攻撃中の場合はコライダーを有効にする
	collider->SetCollisionEnabled(isAttack_);

	//中心座標を設定
	Vector3 center = Mathf::RotateVector(hitbox_.center, enemy->GetDestinationQuaternion());
	collider->SetWorldCenter(enemy->GetHipWorldPosition() + center);

	//敵のトランスフォームを取得
	TransformComponent* enemyTransformComponent = enemy->GetComponent<TransformComponent>();

	//各軸を設定
	collider->SetOrientations(
		{ enemyTransformComponent->worldTransform_.matWorld_.m[0][0], enemyTransformComponent->worldTransform_.matWorld_.m[0][1], enemyTransformComponent->worldTransform_.matWorld_.m[0][2] },
		{ enemyTransformComponent->worldTransform_.matWorld_.m[1][0], enemyTransformComponent->worldTransform_.matWorld_.m[1][1], enemyTransformComponent->worldTransform_.matWorld_.m[1][2] },
		{ enemyTransformComponent->worldTransform_.matWorld_.m[2][0], enemyTransformComponent->worldTransform_.matWorld_.m[2][1], enemyTransformComponent->worldTransform_.matWorld_.m[2][2] }
	);

	//サイズを設定
	collider->SetSize(hitbox_.size);

	//デバッグ表示の設定
	collider->SetDebugDrawEnabled(isDebug_);
}

void Weapon::UpdateObjectNameFromCollider()
{
	//衝突属性マネージャーを取得
	CollisionAttributeManager* collisionAttributeManager = CollisionAttributeManager::GetInstance();
	//コライダーを取得
	OBBCollider* collider = GetComponent<OBBCollider>();
	//コライダーの情報をもとに名前を設定
	name_ = (collider->GetCollisionAttribute() == collisionAttributeManager->GetAttribute("PlayerWeapon")) ? "PlayerWeapon" : "EnemyWeapon";
}

void Weapon::CreateAccelerationField()
{
	auto accelerationField = new AccelerationField();
	accelerationField->Initialize("Weapon", 600.0f);
	accelerationField->SetAcceleration({ 0.0f, -3.8f, 0.0f });
	accelerationField->SetMin({ -50.0f, -50.0f, -50.0f });
	accelerationField->SetMax({ 50.0f, 50.0f, 50.0f });
	particleSystem_->AddAccelerationField(accelerationField);
}

void Weapon::InitializeGlobalVariables()
{
	//環境変数を取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//トランスフォームを取得
	TransformComponent* tranformComponent = GetComponent<TransformComponent>();

	//環境変数の設定
	globalVariables->CreateGroup(name_);
	globalVariables->AddItem(name_, "Translation", tranformComponent->worldTransform_.translation_);
	globalVariables->AddItem(name_, "Rotation", tranformComponent->worldTransform_.rotation_);
	globalVariables->AddItem(name_, "Scale", tranformComponent->worldTransform_.scale_);
	globalVariables->AddItem(name_, "DefaultColliderOffset", defaultColliderOffset_);
	globalVariables->AddItem(name_, "DefaultColliderSize", defaultColliderSize_);
}

void Weapon::ApplyGlobalVariables()
{
	//トランスフォームを取得
	TransformComponent* tranformComponent = GetComponent<TransformComponent>();

	//環境変数の適用
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	tranformComponent->worldTransform_.translation_ = globalVariables->GetVector3Value(name_, "Translation");
	tranformComponent->worldTransform_.rotation_ = globalVariables->GetVector3Value(name_, "Rotation");
	tranformComponent->worldTransform_.scale_ = globalVariables->GetVector3Value(name_, "Scale");
	defaultColliderOffset_ = globalVariables->GetVector3Value(name_, "DefaultColliderOffset");
	defaultColliderSize_ = globalVariables->GetVector3Value(name_, "DefaultColliderSize");
}

void Weapon::UpdateImGui()
{
	//トランスフォームコンポーネントを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//ImGui
	ImGui::Begin(name_.c_str());
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::Checkbox("IsAttack", &isAttack_);
	ImGui::DragFloat3("Translation", &transformComponent->worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transformComponent->worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transformComponent->worldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("HitboxCenter", &hitbox_.center.x, 0.01f);
	ImGui::DragFloat3("HitboxSize", &hitbox_.size.x, 0.01f);
	ImGui::End();
}