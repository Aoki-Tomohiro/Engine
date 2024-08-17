#include "Player.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"

void Player::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//状態の初期化
	ChangeState(new PlayerStateRoot());

	//TransformComponentの初期化
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;
	destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>) * transformComponent->worldTransform_.quaternion_;

	//ModelComponentの初期化
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	for (size_t i = 0; i < modelComponent->GetModel()->GetNumMaterials(); ++i)
	{
		modelComponent->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
	}
	modelComponent->SetAnimationName("Armature|mixamo.com|Layer0");
	//Jointの位置を取得するため一度更新する
	modelComponent->Update();

	//コライダーのオフセット値の初期化
	AABBCollider* collider = GetComponent<AABBCollider>();
	colliderOffset_ = collider->GetCenter() - GetHipLocalPosition();

	//環境変数の設定
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	globalVariables->CreateGroup(groupName);
	globalVariables->AddItem(groupName, "WalkSpeed", rootParameters_.walkSpeed);
	globalVariables->AddItem(groupName, "RunSpeed", rootParameters_.runSpeed);
}

void Player::Update()
{
	//Stateの更新
	state_->Update();

	//アニメーション後の座標を代入
	preAnimationHipPosition_ = GetHipLocalPosition();

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, quaternionInterpolationSpeed_));

	//コライダーの更新
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetCenter(GetHipLocalPosition() + colliderOffset_);

	//Debug時の更新
	DebugUpdate();

	//基底クラスの更新
	GameObject::Update();

	//環境変数の適用
	ApplyGlobalVariables();

	//ImGui
	ImGui::Begin("Player");
	ImGui::Checkbox("IsDebug", &isDebug_);
	ImGui::DragFloat("AnimationTime", &animationTime_, 0.01f);
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
}

void Player::Draw(const Camera& camera)
{
	//基底クラスの描画
	GameObject::Draw(camera);
}

void Player::OnCollision(GameObject* gameObject)
{
	//衝突相手が敵の場合
	if (Enemy* enemy = dynamic_cast<Enemy*>(gameObject))
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

void Player::OnCollisionEnter(GameObject* gameObject)
{
}

void Player::OnCollisionExit(GameObject* gameObject)
{
}

void Player::ChangeState(IPlayerState* newState)
{
	newState->SetPlayer(this);
	newState->Initialize();
	state_.reset(newState);
}

void Player::Move(const Vector3& velocity)
{
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ += velocity * GameTimer::GetDeltaTime();
}

void Player::Rotate(const Vector3& vector)
{
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
	destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>) * Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}

void Player::CorrectAnimationOffset()
{
	Vector3 hipPositionOffset = GetHipLocalPosition() - preAnimationHipPosition_;
	hipPositionOffset.y = 0.0f;
	SetPosition(GetPosition() - hipPositionOffset);
}

void Player::LookAtEnemy()
{
	//プレイヤーの座標を取得
	Vector3 playerPosition = GetPosition();

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetConstGameObject<Enemy>("")->GetHipWorldPosition();

	//敵の方向へのベクトルを計算
	Vector3 rotateVector = Mathf::Normalize(enemyPosition - playerPosition);

	//Y軸にだけ回転させたいのでY成分を0にする
	rotateVector.y = 0.0f;

	//回転処理
	Rotate(Mathf::Normalize(rotateVector));
}

const float Player::GetAnimationTime()
{
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	return modelComponent->GetModel()->GetAnimation()->GetAnimationTime();
}

void Player::SetAnimationName(const std::string& animationName)
{
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	modelComponent->SetAnimationName(animationName);
}

const float Player::GetAnimationSpeed()
{
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	return modelComponent->GetModel()->GetAnimation()->GetAnimationSpeed();
}

void Player::SetAnimationTime(const float animationTime)
{
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(animationTime);
}

void Player::SetAnimationSpeed(const float animationSpeed)
{
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(animationSpeed);
}

void Player::SetIsAnimationLoop(const bool isAnimationLoop)
{
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(isAnimationLoop);
}

void Player::SetIsAnimationStop(const bool isAnimationStop)
{
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetIsStop(isAnimationStop);
}

const bool Player::GetIsAnimationEnd()
{
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	return modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd();
}

const Vector3 Player::GetHipLocalPosition()
{
	//ローカルの腰の座標を取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	Vector3 hipLocalPosition = GetHipWorldPosition() - transformComponent->GetWorldPosition();
	return hipLocalPosition;
}

const Vector3 Player::GetHipWorldPosition()
{
	WorldTransform hipWorldTransform = GetComponent<ModelComponent>()->GetModel()->GetAnimation()->GetJointWorldTransform("mixamorig:Hips");
	Vector3 hipWorldPosition = {
		hipWorldTransform.matWorld_.m[3][0],
		hipWorldTransform.matWorld_.m[3][1],
		hipWorldTransform.matWorld_.m[3][2],
	};
	return hipWorldPosition;
}

const Vector3 Player::GetPosition()
{
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	return transformComponent->worldTransform_.translation_;
}

void Player::SetPosition(const Vector3& position)
{
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.translation_ = position;
}

void Player::DebugUpdate()
{
	if (isDebug_)
	{
		ModelComponent* modelComponent = GetComponent<ModelComponent>();
		modelComponent->SetAnimationName(currentAnimationName_);
		modelComponent->GetModel()->GetAnimation()->SetIsStop(true);
		modelComponent->GetModel()->GetAnimation()->SetAnimationTime(animationTime_);
	}
}

void Player::ApplyGlobalVariables()
{
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	rootParameters_.walkSpeed = globalVariables->GetFloatValue(groupName, "WalkSpeed");
	rootParameters_.runSpeed = globalVariables->GetFloatValue(groupName, "RunSpeed");
}