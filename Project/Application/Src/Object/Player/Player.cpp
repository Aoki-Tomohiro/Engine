#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Animator/AnimatorComponent.h"
#include "Engine/Components/Model/ModelComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/Transform/TransformComponent.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"

void Player::Initialize()
{
	//基底クラスの初期化
	GameObject::Initialize();

	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//状態の初期化
	ChangeState(new PlayerStateRoot());

	//トランスフォームコンポーネントを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	//回転の種類をQuaternionに設定
	transformComponent->worldTransform_.rotationType_ = RotationType::Quaternion;

	//モデルコンポーネントを取得
	ModelComponent* modelComponent = GetComponent<ModelComponent>();
	//全てのマテリアルの環境マップの映り込みの係数を設定
	for (size_t i = 0; i < modelComponent->GetModel()->GetNumMaterials(); ++i)
	{
		modelComponent->GetModel()->GetMaterial(i)->SetEnvironmentCoefficient(0.0f);
	}

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

	//回転処理
	UpdateRotation();

	//コライダーの更新
	UpdateCollider();

	//デバッグ時の更新処理
	DebugUpdate();

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

void Player::OnCollision(GameObject* gameObject)
{
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

void Player::Rotate(const Vector3& vector)
{
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
	destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion({ 0.0f,1.0f,0.0f }, std::numbers::pi_v<float>) * Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
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

void Player::SetIsAnimationBlending(const bool isBlending)
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->SetIsBlending(isBlending);
}

void Player::SetAnimationBlendDuration(const float blendDuration)
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	animatorComponent->SetBlendDuration(blendDuration);
}

const float Player::GetAnimationTime()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetAnimationTime();
}

const bool Player::GetIsAnimationFinished()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetIsAnimationFinished();
}

const float Player::GetAnimationDuration()
{
	AnimatorComponent* animatorComponent = GetComponent<AnimatorComponent>();
	return animatorComponent->GetAnimationDuration();
}

const Vector3 Player::GetHipLocalPosition()
{
	//トランスフォームコンポーネントを取得
	TransformComponent* transformComponent = GetComponent<TransformComponent>();

	//腰のジョイントのローカル座標を計算
	Vector3 hipLocalPosition = GetHipWorldPosition() - transformComponent->GetWorldPosition();

	//腰のジョイントのローカル座標を返す
	return hipLocalPosition;
}

const Vector3 Player::GetHipWorldPosition()
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

	//コライダーの中心を更新
	collider->SetCenter(GetHipLocalPosition() + colliderOffset_);
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