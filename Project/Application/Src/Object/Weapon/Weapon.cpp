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

	//モデルの初期化
	InitializeModel();

	//コライダーの初期化
	InitializeCollider();

	//パーティクルシステムの初期化
	InitializeParticleSystem();

	//加速フィールドの生成
	CreateAccelerationField();

	//環境変数の初期化
	InitializeGlobalVariables();

	//軌跡の作成
	TextureManager::Load("Trail.png");
	trail_ = TrailRenderer::CreateTrail();
	trail_->SetTexture("Trail.png");

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

	//軌跡の更新
	UpdateTrail();

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
		Vector3 emitterTranslation = GetComponent<OBBCollider>()->GetWorldCenter();
		//ヒットエフェクト
		ParticleEmitter* hitNewEmitter = EmitterBuilder()
			.SetEmitterName("Hit")
			.SetEmitterLifeTime(1.0f)
			.SetTranslation(emitterTranslation)
			.SetCount(200)
			.SetColor({ 1.0f, 0.2f, 0.2f, 1.0f }, { 1.0f, 0.2f, 0.2f, 1.0f })
			.SetFrequency(2.0f)
			.SetLifeTime(0.2f, 0.4f)
			.SetRadius(0.0f)
			.SetScale({ 0.2f,0.2f,0.2f }, { 0.3f,0.3f,0.3f })
			.SetVelocity({ -0.6f,-0.6f,-0.6f }, { 0.6f,0.6f,0.6f })
			.Build();
		particleSystems_["Hit"]->AddParticleEmitter(hitNewEmitter);

		//星のパーティクル
		ParticleEmitter* starNewEmitter = EmitterBuilder()
			.SetEmitterName("Hit")
			.SetEmitterLifeTime(0.1f)
			.SetTranslation(emitterTranslation)
			.SetCount(1)
			.SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f })
			.SetFrequency(2.0f)
			.SetLifeTime(0.2f, 0.4f)
			.SetRadius(0.0f)
			.SetScale({ 1.4f,1.4f,1.4f }, { 1.4f,1.4f,1.4f })
			.SetRotate({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.SetVelocity({ 0.0f,0.0f,0.0f }, { 0.0f,0.0f,0.0f })
			.Build();
		particleSystems_["Star"]->AddParticleEmitter(starNewEmitter);
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

void Weapon::UpdateTrail()
{
	//軌跡が有効な場合
	if (isTrailActive_)
	{
		//トランスフォームを取得
		TransformComponent* tranformComponent = GetComponent<TransformComponent>();

		//ワールド座標を取得
		Vector3 worldPosition = tranformComponent->GetWorldPosition();

		//オフセット値を計算
		Vector3 headOffset = Mathf::TransformNormal(headOffset_, tranformComponent->worldTransform_.matWorld_);
		Vector3 frontOffset = Mathf::TransformNormal(frontOffset_, tranformComponent->worldTransform_.matWorld_);

		//軌跡の追加
		trail_->AddVertex(worldPosition + headOffset, worldPosition + frontOffset);
	}

	//軌跡の色の更新
	trail_->SetColor(trailColor_);
}

void Weapon::InitializeModel()
{
	//モデルの初期化
	ModelComponent* modelComponent = AddComponent<ModelComponent>();
	modelComponent->SetModel(name_ == "PlayerWeapon" ? ModelManager::CreateFromModelFile("PlayerWeapon", Opaque) : ModelManager::CreateFromModelFile("EnemyWeapon", Opaque));
}

void Weapon::InitializeCollider()
{
	//コライダーの追加
	OBBCollider* collider = AddComponent<OBBCollider>();
	CollisionAttributeManager* collisionAttributeManager = CollisionAttributeManager::GetInstance();
	collider->SetCollisionAttribute(name_ == "PlayerWeapon" ? collisionAttributeManager->GetAttribute("PlayerWeapon") : collisionAttributeManager->GetAttribute("EnemyWeapon"));
	collider->SetCollisionMask(name_ == "PlayerWeapon" ? collisionAttributeManager->GetMask("PlayerWeapon") : collisionAttributeManager->GetMask("EnemyWeapon"));
}

void Weapon::InitializeParticleSystem()
{
	//パーティクルシステムの作成
	TextureManager::Load("star_09.png");
	particleSystems_["Hit"] = ParticleManager::Create("Hit");
	particleSystems_["Star"] = ParticleManager::Create("Star");
	particleSystems_["Star"]->SetTexture("star_09.png");
}

void Weapon::CreateAccelerationField()
{
	auto accelerationField = new AccelerationField();
	accelerationField->Initialize("Weapon", 600.0f);
	accelerationField->SetAcceleration({ 0.0f, -3.8f, 0.0f });
	accelerationField->SetMin({ -50.0f, -50.0f, -50.0f });
	accelerationField->SetMax({ 50.0f, 50.0f, 50.0f });
	particleSystems_["Hit"]->AddAccelerationField(accelerationField);
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
	ImGui::ColorEdit4("TrailColor", &trailColor_.x);
	ImGui::End();
}