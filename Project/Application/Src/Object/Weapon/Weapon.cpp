#include "Weapon.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include <numbers>

void Weapon::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//トランスフォームの初期化
	InitializeTransform();

	//モデルの初期化
	InitializeModel();

	//コライダーの初期化
	InitializeCollider();

	//パーティクルの初期化
	InitializeParticleSystem();

	//加速フィールドの作成
	CreateAccelerationField();

	//環境変数の初期化
	InitializeGlobalVariables();

	//軌跡の初期化
	InitializeTrail();

	//音声データの初期化
	InitializeHitAudio();
}

void Weapon::Update()
{
	//ヒットフラグのリセット
	isHit_ = false;

	//コライダーの更新
	UpdateCollider();

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

void Weapon::OnCollision(GameObject* gameObject)
{
	//ヒットフラグを立てる
	isHit_ = true;

	//プレイヤーの武器の場合は各エフェクトを発生させる
	if (name_ == "PlayerWeapon")
	{
		HandlePlayerWeaponCollision();
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
	//ヒットボックスのサイズと中心を設定
	hitbox_.center = hitbox.center;
	hitbox_.size = hitbox.size;
}

void Weapon::InitializeTransform()
{
	//トランスフォームの初期化
	transform_ = GetComponent<TransformComponent>();

	//武器の親オブジェクトを取得し、親子付け
	GameObject* parentGameObject = (name_ == "PlayerWeapon") ?
		GameObjectManager::GetInstance()->GetMutableGameObject<GameObject>("Player") :
		GameObjectManager::GetInstance()->GetMutableGameObject<GameObject>("Enemy");

	//親オブジェクトの右手に武器を設定
	transform_->worldTransform_.SetParent(&parentGameObject->GetComponent<ModelComponent>()->GetModel()->GetJointWorldTransform("mixamorig:RightHand"));
}

void Weapon::InitializeModel()
{
	//武器のモデルを初期化
	std::string modelFileName = (name_ == "PlayerWeapon") ? "PlayerWeapon" : "EnemyWeapon";
	model_ = AddComponent<ModelComponent>();
	model_->SetModel(ModelManager::CreateFromModelFile(modelFileName, Opaque));
}

void Weapon::InitializeCollider()
{
	//コライダーの初期化
	collider_ = AddComponent<OBBCollider>();
	CollisionAttributeManager* collisionAttrManager = CollisionAttributeManager::GetInstance();
	uint32_t collisionAttribute = name_ == "PlayerWeapon" ? collisionAttrManager->GetAttribute("PlayerWeapon") : collisionAttrManager->GetAttribute("EnemyWeapon");
	uint32_t collisionMask = name_ == "PlayerWeapon" ? collisionAttrManager->GetMask("PlayerWeapon") : collisionAttrManager->GetMask("EnemyWeapon");
	collider_->SetCollisionAttribute(collisionAttribute);
	collider_->SetCollisionMask(collisionMask);
}

void Weapon::InitializeParticleSystem()
{
	//テクスチャの読み込み
	TextureManager::Load("star_08.png");

	//モデルの生成
	sparkParticleModel_.reset(ModelManager::CreateFromModelFile("Cube", Transparent));

	//衝撃波パーティクルの生成
	particleSystems_["ShockWave"] = ParticleManager::Create("ShockWave");

	//火花のパーティクル生成
	particleSystems_["Spark"] = ParticleManager::Create("Spark");
	particleSystems_["Spark"]->SetModel(sparkParticleModel_.get());

	//光のパーティクル生成
	particleSystems_["Light"] = ParticleManager::Create("Light");
	particleSystems_["Light"]->SetTexture("star_08.png");
}

void Weapon::CreateAccelerationField()
{
	//加速フィールドの作成
	AccelerationField* accelerationField = new AccelerationField();
	accelerationField->Initialize("Weapon", 600.0f);
	accelerationField->SetAcceleration({ 0.0f, -3.8f, 0.0f });
	accelerationField->SetMin({ -50.0f, -50.0f, -50.0f });
	accelerationField->SetMax({ 50.0f, 50.0f, 50.0f });
	particleSystems_["Spark"]->AddAccelerationField(accelerationField);
}

void Weapon::InitializeGlobalVariables()
{
	//環境変数を初期化
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	globalVariables->CreateGroup(name_);
	globalVariables->AddItem(name_, "Translation", transform_->worldTransform_.translation_);
	globalVariables->AddItem(name_, "Rotation", transform_->worldTransform_.rotation_);
	globalVariables->AddItem(name_, "Scale", transform_->worldTransform_.scale_);
	globalVariables->AddItem(name_, "DefaultColliderOffset", defaultColliderOffset_);
	globalVariables->AddItem(name_, "DefaultColliderSize", defaultColliderSize_);
}

void Weapon::InitializeTrail()
{
	//軌跡の初期化
	TextureManager::Load("Trail.png");
	trail_ = TrailRenderer::CreateTrail();
	trail_->SetTexture("Trail.png");
}

void Weapon::InitializeHitAudio()
{
	//ヒット時の音声データを初期化
	hitAudioHandle_ = audio_->LoadAudioFile("Hit.mp3");
}

void Weapon::UpdateCollider()
{
	//親オブジェクトを取得してコライダーを更新
	GameObject* parentGameObject = (name_ == "PlayerWeapon") ?
		GameObjectManager::GetInstance()->GetMutableGameObject<GameObject>("Player") :
		GameObjectManager::GetInstance()->GetMutableGameObject<GameObject>("Enemy");

	//コライダーの有効状態を設定
	collider_->SetCollisionEnabled(isAttack_);

	//コライダーのトランスフォームを設定
	SetColliderTransform(parentGameObject);
}

void Weapon::SetColliderTransform(GameObject* gameObject)
{
	//親オブジェクトのトランスフォームを基にコライダーの座標とサイズを設定
	TransformComponent* parentTransform = gameObject->GetComponent<TransformComponent>();

	//親の腰のワールド座標を取得
	WorldTransform hipWorldTransform = gameObject->GetComponent<ModelComponent>()->GetModel()->GetJointWorldTransform("mixamorig:Hips");
	Vector3 hipWorldPosition = { hipWorldTransform.matWorld_.m[3][0], hipWorldTransform.matWorld_.m[3][1], hipWorldTransform.matWorld_.m[3][2] };

	//コライダーの中心と向き、サイズを設定
	Vector3 colliderCenter = Mathf::RotateVector(hitbox_.center, parentTransform->worldTransform_.quaternion_);
	collider_->SetWorldCenter(hipWorldPosition + colliderCenter);
	collider_->SetOrientations(
		{ parentTransform->worldTransform_.matWorld_.m[0][0], parentTransform->worldTransform_.matWorld_.m[0][1], parentTransform->worldTransform_.matWorld_.m[0][2] },
		{ parentTransform->worldTransform_.matWorld_.m[1][0], parentTransform->worldTransform_.matWorld_.m[1][1], parentTransform->worldTransform_.matWorld_.m[1][2] },
		{ parentTransform->worldTransform_.matWorld_.m[2][0], parentTransform->worldTransform_.matWorld_.m[2][1], parentTransform->worldTransform_.matWorld_.m[2][2] }
	);
	collider_->SetSize(hitbox_.size);

	//デバッグ表示の設定
	collider_->SetDebugDrawEnabled(isDebug_);
}

void Weapon::UpdateTrail()
{
	//軌跡が有効な場合
	if (isTrailActive_)
	{
		//軌跡の座標と色を更新
		Vector3 worldPosition = transform_->GetWorldPosition();
		Vector3 headOffset = Mathf::TransformNormal(headOffset_, transform_->worldTransform_.matWorld_);
		Vector3 frontOffset = Mathf::TransformNormal(frontOffset_, transform_->worldTransform_.matWorld_);
		trail_->AddVertex(worldPosition + headOffset, worldPosition + frontOffset);
		trail_->SetColor(trailColor_);
	}
}

void Weapon::ApplyGlobalVariables()
{
	//環境変数を設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	transform_->worldTransform_.translation_ = globalVariables->GetVector3Value(name_, "Translation");
	transform_->worldTransform_.rotation_ = globalVariables->GetVector3Value(name_, "Rotation");
	transform_->worldTransform_.scale_ = globalVariables->GetVector3Value(name_, "Scale");
	defaultColliderOffset_ = globalVariables->GetVector3Value(name_, "DefaultColliderOffset");
	defaultColliderSize_ = globalVariables->GetVector3Value(name_, "DefaultColliderSize");
}

void Weapon::UpdateImGui()
{
	//ImGuiの更新
	ImGui::Begin(name_.c_str());
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::Checkbox("IsAttack", &isAttack_);
	ImGui::DragFloat3("Translation", &transform_->worldTransform_.translation_.x, 0.01f);
	ImGui::DragFloat3("Rotation", &transform_->worldTransform_.rotation_.x, 0.01f);
	ImGui::DragFloat3("Scale", &transform_->worldTransform_.scale_.x, 0.01f);
	ImGui::DragFloat3("HitboxCenter", &hitbox_.center.x, 0.01f);
	ImGui::DragFloat3("HitboxSize", &hitbox_.size.x, 0.01f);
	ImGui::ColorEdit4("TrailColor", &trailColor_.x);
	ImGui::End();
}

void Weapon::HandlePlayerWeaponCollision()
{
	//ヒットストップを開始
	hitStop_->Start(effectSettings_.hitStopDuration);

	//ヒット音を再生
	audio_->PlayAudio(hitAudioHandle_, false, 0.2f);

	//エミッターを生成
	CreateParticleEmitters();
}

void Weapon::CreateParticleEmitters()
{
	//エミッターの座標を取得
	Vector3 emitterTranslation = transform_->GetWorldPosition();

	//カメラを取得
	Camera* camera = CameraManager::GetCamera("Camera");

	//衝撃波のエミッターの生成
	CreateShockWaveEmitters(emitterTranslation, camera);

	//火花のエミッターの生成
	CreateSparkEmitter(emitterTranslation);

	//光のエミッターの生成
	CreateStarEmitter(emitterTranslation);
}

void Weapon::CreateShockWaveEmitters(const Vector3& emitterTranslation, Camera* camera)
{
	//パーティクルの速度
	const float kParticleVelocity = 0.06f;

	//円状になるように360度分のエミッターを生成
	for (int32_t i = 0; i < 360; ++i)
	{
		//Quaternionを計算
		Quaternion quaternion = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f, 0.0f, 1.0f }, static_cast<float>(i) * (std::numbers::pi_v<float> / 180.0f));

		//速度を計算
		Vector3 velocity = Mathf::RotateVector({ 0.0f, 1.0f, 0.0f }, camera->quaternion_ * quaternion) * kParticleVelocity;

		//オフセット値を計算
		Vector3 offset = Mathf::RotateVector({ 0.0f, 1.4f, 0.0f }, camera->quaternion_ * quaternion);

		//エミッターを生成
		ParticleEmitter* hitEmitter = CreateShockWaveEmitter(emitterTranslation + offset, velocity);

		//パーティクルシステムに追加
		particleSystems_["ShockWave"]->AddParticleEmitter(hitEmitter);
	}
}

ParticleEmitter* Weapon::CreateShockWaveEmitter(const Vector3& position, const Vector3& velocity)
{
	return EmitterBuilder()
		.SetEmitterName("ShockWave")
		.SetEmitterLifeTime(0.0f)
		.SetTranslation(position)
		.SetCount(1)
		.SetColor({ 1.0f, 0.2f, 0.2f, 1.0f }, { 1.0f, 0.2f, 0.2f, 1.0f })
		.SetFrequency(2.0f)
		.SetLifeTime(0.2f, 0.2f)
		.SetRadius(0.0f)
		.SetScale({ 0.1f, 0.1f, 0.1f }, { 0.1f, 0.1f, 0.1f })
		.SetVelocity(velocity, velocity)
		.Build();
}

void Weapon::CreateSparkEmitter(const Vector3& emitterTranslation)
{
	//パーティクルの速度
	const float kParticleVelocity = 0.2f;

	//エミッターを生成
	ParticleEmitter* emitter = EmitterBuilder()
		.SetEmitterName("Spark")
		.SetEmitterLifeTime(0.0f)
		.SetTranslation(emitterTranslation)
		.SetCount(200)
		.SetColor({ 1.0f, 0.2f, 0.2f, 1.0f }, { 1.0f, 0.2f, 0.2f, 1.0f })
		.SetFrequency(2.0f)
		.SetLifeTime(0.2f, 0.4f)
		.SetRadius(0.0f)
		.SetScale({ 0.01f, 0.01f, 0.1f }, { 0.02f, 0.02f, 0.2f })
		.SetVelocity({ -kParticleVelocity, -kParticleVelocity, -kParticleVelocity }, { kParticleVelocity, kParticleVelocity, kParticleVelocity })
		.SetAlignToDirection(true)
		.Build();

	//パーティクルシステムのエミッターを追加
	particleSystems_["Spark"]->AddParticleEmitter(emitter);
}

void Weapon::CreateStarEmitter(const Vector3& position)
{
	//エミッターを生成
	ParticleEmitter* emitter = EmitterBuilder()
		.SetEmitterName("Light")
		.SetEmitterLifeTime(0.1f)
		.SetTranslation(position)
		.SetCount(6)
		.SetColor({ 1.0f, 1.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 1.0f, 1.0f })
		.SetFrequency(2.0f)
		.SetLifeTime(0.2f, 0.4f)
		.SetRadius(0.0f)
		.SetScale({ 2.0f, 2.0f, 1.4f }, { 2.0f, 2.0f, 2.0f })
		.SetRotate({ 0.0f, 0.0f, -std::numbers::pi_v<float> }, { 0.0f, 0.0f, std::numbers::pi_v<float> })
		.SetVelocity({ 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, 0.0f })
		.Build();

	//パーティクルシステムのエミッターを追加
	particleSystems_["Light"]->AddParticleEmitter(emitter);
}