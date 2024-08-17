#include "Weapon.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Collision/OBBCollider.h"
#include "Engine/Components/Collision/CollisionAttributeManager.h"
#include "Application/Src/Object/Player/Player.h"

void Weapon::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//Audioのインスタンスを取得
	audio_ = Audio::GetInstance();

	//パーティクルシステムの作成
	particleSystem_ = ParticleManager::Create("Weapon");
	AccelerationField* accelerationField = new AccelerationField();
	accelerationField->Initialize("Weapon", 600.0f);
	accelerationField->SetAcceleration({ 0.0f,-3.8f,0.0f });
	accelerationField->SetMin({ -50.0f,-50.0f,-50.0f });
	accelerationField->SetMax({ 50.0f,50.0f,50.0f });
	particleSystem_->AddAccelerationField(accelerationField);

	//GlobalVariablesを取得
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	//衝突属性マネージャーを取得
	CollisionAttributeManager* collisionAttributeManager = CollisionAttributeManager::GetInstance();
	//Colliderを取得
	OBBCollider* collider = GetComponent<OBBCollider>();
	//コライダーの属性から名前を決める
	name_ = collider->GetCollisionAttribute() == collisionAttributeManager->GetAttribute("PlayerWeapon") ? "PlayerWeapon" : "EnemyWeapon";
	//環境変数の設定
	globalVariables->CreateGroup(name_);
	globalVariables->AddItem(name_, "ColliderOffset", colliderOffset_);
	globalVariables->AddItem(name_, "ColliderSize", colliderSize_);
}

void Weapon::Update()
{
	//フラグのリセット
	isHit_ = false;

	//コライダーの更新
	UpdateCollider();

	//基底クラスの更新
	GameObject::Update();

	//環境変数の適用
	ApplyGlobalVariables();

	//トランスフォームを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//ImGui
	ImGui::Begin(name_.c_str());
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::Checkbox("IsAttack", &isAttack_);
	ImGui::DragFloat3("Translation", &transformComponent->worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transformComponent->worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transformComponent->worldTransform_.scale_.x, 0.01f);
	ImGui::End();
}

void Weapon::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void Weapon::OnCollision(GameObject* gameObject)
{
	//ヒットフラグを立てる
	isHit_ = true;

	//パーティクルを出す
	OBBCollider* collider = GetComponent<OBBCollider>();
	ParticleEmitter* newEmitter = new ParticleEmitter();
	newEmitter->Initialize("Hit", 1.0f);
	newEmitter->SetTranslate(collider->GetWorldCenter());
	newEmitter->SetCount(400);
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

void Weapon::OnCollisionEnter(GameObject* gameObject)
{
}

void Weapon::OnCollisionExit(GameObject* gameObject)
{
}

void Weapon::UpdateCollider()
{
	//プレイヤーのトランスフォームを取得
	Player* player = GameObjectManager::GetInstance()->GetMutableGameObject<Player>("");
	TransformComponent* playerTransformComponent = player->GetComponent<TransformComponent>();

	//プレイヤーのコライダーを取得
	AABBCollider* playerCollider = player->GetComponent<AABBCollider>();

	//コライダーを取得
	OBBCollider* collider = GetComponent<OBBCollider>();

	//攻撃中の場合はコライダーを有効にする
	collider->SetCollisionEnabled(isAttack_);

	//中心座標を設定
	Vector3 center = Mathf::RotateVector(colliderOffset_, player->GetDestinationQuaternion());
	collider->SetCenter(playerCollider->GetCenter() + center);

	//各軸を設定
	collider->SetOrientations(
		{ playerTransformComponent->worldTransform_.matWorld_.m[0][0], playerTransformComponent->worldTransform_.matWorld_.m[0][1], playerTransformComponent->worldTransform_.matWorld_.m[0][2] },
		{ playerTransformComponent->worldTransform_.matWorld_.m[1][0], playerTransformComponent->worldTransform_.matWorld_.m[1][1], playerTransformComponent->worldTransform_.matWorld_.m[1][2] },
		{ playerTransformComponent->worldTransform_.matWorld_.m[2][0], playerTransformComponent->worldTransform_.matWorld_.m[2][1], playerTransformComponent->worldTransform_.matWorld_.m[2][2] }
	);

	//サイズを設定
	collider->SetSize(colliderSize_);

	//デバッグ表示の設定
	collider->SetDebugDrawEnabled(isDebug_);
}

void Weapon::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	colliderOffset_ = globalVariables->GetVector3Value(name_, "ColliderOffset");
	colliderSize_ = globalVariables->GetVector3Value(name_, "ColliderSize");
}