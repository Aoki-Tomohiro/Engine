#include "Weapon.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include <numbers>

void Weapon::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//トランスフォームの初期化
	InitializeTransform();

	//モデルの初期化
	InitializeModel();

	//コライダーの初期化
	InitializeCollider();

	//環境変数の初期化
	InitializeGlobalVariables();

	//軌跡の初期化
	InitializeTrail();
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

	//ImGuiの更新
	UpdateImGui();

	//環境変数の適用
	ApplyGlobalVariables();
}

void Weapon::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void Weapon::OnCollision(GameObject*)
{
	//ヒットフラグを立てる
	isHit_ = true;
}

void Weapon::InitializeTransform()
{
	//トランスフォームの初期化
	transform_ = GetComponent<TransformComponent>();

	//武器の親オブジェクトを取得し、親子付け
	GameObject* parentGameObject = (name_ == "PlayerWeapon") ?
		gameObjectManager_->GetGameObject<GameObject>("Player") :
		gameObjectManager_->GetGameObject<GameObject>("Enemy");

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
	TextureManager::Load("Trail2.png");
	trail_ = TrailRenderer::CreateTrail(name_);
	trail_->SetTexture("Trail2.png");
}

void Weapon::UpdateCollider()
{
	//親オブジェクトを取得してコライダーを更新
	GameObject* parentGameObject = (name_ == "PlayerWeapon") ? gameObjectManager_->GetGameObject<GameObject>("Player") : gameObjectManager_->GetGameObject<GameObject>("Enemy");

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
	Vector3 colliderCenter = Mathf::RotateVector(hitboxParameters_.center, parentTransform->worldTransform_.quaternion_);
	collider_->SetWorldCenter(hipWorldPosition + colliderCenter);
	collider_->SetOrientations(
		{ parentTransform->worldTransform_.matWorld_.m[0][0], parentTransform->worldTransform_.matWorld_.m[0][1], parentTransform->worldTransform_.matWorld_.m[0][2] },
		{ parentTransform->worldTransform_.matWorld_.m[1][0], parentTransform->worldTransform_.matWorld_.m[1][1], parentTransform->worldTransform_.matWorld_.m[1][2] },
		{ parentTransform->worldTransform_.matWorld_.m[2][0], parentTransform->worldTransform_.matWorld_.m[2][1], parentTransform->worldTransform_.matWorld_.m[2][2] }
	);
	collider_->SetSize(hitboxParameters_.size);

	//デバッグ表示の設定
	collider_->SetDebugDrawEnabled(isDebug_);
}

void Weapon::UpdateTrail()
{
	//軌跡が有効な場合
	if (isTrailActive_)
	{
		//タイマーを進める
		addVertexTimer_ += GameTimer::GetDeltaTime();

		//タイマーが一定値を超えていた場合
		if (addVertexTimer_ > trailVertexInterval_)
		{
			//タイマーのリセット
			addVertexTimer_ = 0.0f;

			//軌跡の頂点を追加
			Vector3 worldPosition = transform_->GetWorldPosition();
			Vector3 headOffset = Mathf::TransformNormal(headOffset_, transform_->worldTransform_.matWorld_);
			Vector3 frontOffset = Mathf::TransformNormal(frontOffset_, transform_->worldTransform_.matWorld_);
			trail_->AddTrail(worldPosition + headOffset, worldPosition + frontOffset);
		}

		//軌跡の色を更新
		trail_->SetStartColor(trailStartColor_);
		trail_->SetEndColor(trailEndColor_);
	}
	else
	{
		//タイマーのリセット
		addVertexTimer_ = 0.0f;
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
	ImGui::DragFloat3("HeadOffset", &headOffset_.x);
	ImGui::DragFloat3("FrontOffset", &frontOffset_.x);
	ImGui::ColorEdit4("TrailStartColor", &trailStartColor_.x);
	ImGui::ColorEdit4("TrailEndColor", &trailEndColor_.x);
	ImGui::End();
}