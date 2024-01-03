#include "Player.h"
#include "Engine/3D/ModelManager.h"
#include "Engine/Framework/GameObjectManager.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Components/CollisionConfig.h"

void Player::Initialize() 
{
	//基底クラスの初期化
	IGameObject::Initialize();
	worldTransform_.translation_.y = 1.0f;
	worldTransform_.translation_.z = -20.0f;

	//入力クラスのインスタンスを取得
	input_ = Input::GetInstance();

	//武器の生成
	modelWeapon_ = ModelManager::CreateFromOBJ("Weapon", Opaque);
	weapon_ = GameObjectManager::CreateGameObject<Weapon>();
	weapon_->SetModel(modelWeapon_);
	weapon_->SetPlayerWorldTransform(&worldTransform_);

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Player::Update() 
{
	//ダッシュのクールタイム
	if (workDash_.coolTime != workDash_.dashCoolTime)
	{
		workDash_.coolTime++;
	}

	//Behaviorの遷移処理
	if (behaviorRequest_) 
	{
		//振る舞いを変更する
		behavior_ = behaviorRequest_.value();
		//各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			BehaviorRootInitialize();
			break;
		case Behavior::kDash:
			BehaviorDashInitialize();
			break;
		case Behavior::kJump:
			BehaviorJumpInitialize();
			break;
		case Behavior::kAttack:
			BehaviorAttackInitialize();
			break;
		case Behavior::kKnockBack:
			BehaviorKnockBackInitialize();
			break;
		case Behavior::kRapidApproach:
			BehaviorRapidApproachInitialize();
			break;
		}
		behaviorRequest_ = std::nullopt;
	}

	//Behaviorの実行
	switch (behavior_)
	{
	case Behavior::kRoot:
	default:
		BehaviorRootUpdate();
		break;
	case Behavior::kDash:
		BehaviorDashUpdate();
		break;
	case Behavior::kJump:
		BehaviorJumpUpdate();
		break;
	case Behavior::kAttack:
		BehaviorAttackUpdate();
		break;
	case Behavior::kKnockBack:
		BehaviorKnockBackUpdate();
		break;
	case Behavior::kRapidApproach:
		BehaviorRapidApproachUpdate();
		break;
	}

	//移動限界座標
	const float kMoveLimitX = 49;
	const float kMoveLimitZ = 49;
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);
	worldTransform_.translation_.z = max(worldTransform_.translation_.z, -kMoveLimitZ);
	worldTransform_.translation_.z = min(worldTransform_.translation_.z, +kMoveLimitZ);

	//ワールドトランスフォームの更新
	worldTransform_.quaternion_ = Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f);
	worldTransform_.UpdateMatrixFromQuaternion();
}

void Player::Draw(const Camera& camera) 
{
	//基底クラスの描画
	IGameObject::Draw(camera);
	
	//武器の描画
	if (behavior_ == Behavior::kAttack) 
	{
		weapon_->SetIsVisible(true);
	}
	else
	{
		weapon_->SetIsVisible(false);
	}
}

void Player::UpdateParticle()
{
	//武器のパーティクルの更新
	weapon_->UpdateParticle();
}

void Player::DrawParticle(const Camera& camera)
{
	//武器のパーティクルの描画
	weapon_->DrawParticle(camera);
}

void Player::OnCollision(Collider* collider)
{
	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy)
	{
		//Vector3 sub = GetWorldPosition() - collider->GetWorldPosition();
		//Vector3 kKnockBackSpeed = { 1.0f,sub.y < 0.0f ? -1.0f : 1.0f ,1.0f };
		//knockBackVelocity_ = Mathf::Normalize(sub) * kKnockBackSpeed;
		//behaviorRequest_ = Behavior::kKnockBack;

		AABB aabbA = {
            .min{worldTransform_.translation_.x + GetAABB().min.x,worldTransform_.translation_.y + GetAABB().min.y,worldTransform_.translation_.z + GetAABB().min.z},
            .max{worldTransform_.translation_.x + GetAABB().max.x,worldTransform_.translation_.y + GetAABB().max.y,worldTransform_.translation_.z + GetAABB().max.z},
		};
		AABB aabbB = {
			.min{collider->GetWorldTransform().translation_.x + collider->GetAABB().min.x,collider->GetWorldTransform().translation_.y + collider->GetAABB().min.y,collider->GetWorldTransform().translation_.z + collider->GetAABB().min.z},
			.max{collider->GetWorldTransform().translation_.x + collider->GetAABB().max.x,collider->GetWorldTransform().translation_.y + collider->GetAABB().max.y,collider->GetWorldTransform().translation_.z + collider->GetAABB().max.z},
		};

		Vector3 overlapAxis = {
			std::min<float>(aabbA.max.x,aabbB.max.x) - std::max<float>(aabbA.min.x,aabbB.min.x),
			std::min<float>(aabbA.max.y,aabbB.max.y) - std::max<float>(aabbA.min.y,aabbB.min.y),
			std::min<float>(aabbA.max.z,aabbB.max.z) - std::max<float>(aabbA.min.z,aabbB.min.z),
		};

		Vector3 directionAxis{};
		if (overlapAxis.x < overlapAxis.y && overlapAxis.x < overlapAxis.z) {
			//X軸方向で最小の重なりが発生している場合
			directionAxis.x = (worldTransform_.translation_.x < collider->GetWorldTransform().translation_.x) ? -1.0f : 1.0f;
			directionAxis.y = 0.0f;
		}
		else if (overlapAxis.y < overlapAxis.x && overlapAxis.y < overlapAxis.z) {
			//Y軸方向で最小の重なりが発生している場合
			directionAxis.y = (worldTransform_.translation_.y < collider->GetWorldTransform().translation_.y) ? -1.0f : 1.0f;
			directionAxis.x = 0.0f;
		}
		else if (overlapAxis.z < overlapAxis.x && overlapAxis.z < overlapAxis.y)
		{
			directionAxis.z = (worldTransform_.translation_.z < collider->GetWorldTransform().translation_.z) ? -1.0f : 1.0f;
			directionAxis.x = 0.0f;
			directionAxis.y = 0.0f;
		}

		worldTransform_.translation_ += overlapAxis * directionAxis;
		worldTransform_.UpdateMatrixFromQuaternion();
	}
}

Vector3 Player::GetWorldPosition() 
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}

void Player::UpdateImGui()
{
	ImGui::Begin("Player");
	ImGui::Text("RB : Dash");
	ImGui::Text("A : Jump");
	ImGui::Text("X : Attack");
	ImGui::Text("ComoIndex : %d", workAttack_.comboIndex);
	if (behavior_ == Behavior::kRoot)
	{
		ImGui::Text("Behavior : kRoot");
	}
	else if (behavior_ == Behavior::kDash)
	{
		ImGui::Text("Behavior : kDash");
	}
	else if (behavior_ == Behavior::kJump)
	{
		ImGui::Text("Behavior : kJump");
	}
	else if (behavior_ == Behavior::kAttack)
	{
		ImGui::Text("Behavior : kAttack");
	}
	ImGui::SliderInt("AttackIndex", &attackIndex_, 0, ComboNum);
	ImGui::DragFloat3("StartPosition", &kConstAttacks_[attackIndex_].startPosition.x, 0.01f);
	ImGui::DragFloat3("StartRotation", &kConstAttacks_[attackIndex_].startRotation.x, 0.01f);
	ImGui::DragInt("AnticipationTime", reinterpret_cast<int*>(&kConstAttacks_[attackIndex_].anticipationTime));
	ImGui::DragFloat3("AnticipationSpeed", &kConstAttacks_[attackIndex_].anticipationSpeed.x, 0.01f);
	ImGui::DragFloat3("AnticipationRotateSpeed", &kConstAttacks_[attackIndex_].anticipationRotateSpeed.x, 0.01f);
	ImGui::DragInt("ChageTime", reinterpret_cast<int*>(&kConstAttacks_[attackIndex_].chargeTime));
	ImGui::DragFloat3("ChargeSpeed", &kConstAttacks_[attackIndex_].chargeSpeed.x, 0.01f);
	ImGui::DragFloat3("ChargeRotateSpeed", &kConstAttacks_[attackIndex_].chargeRotateSpeed.x, 0.01f);
	ImGui::DragInt("SwingTime", reinterpret_cast<int*>(&kConstAttacks_[attackIndex_].swingTime));
	ImGui::DragFloat3("SwingSpeed", &kConstAttacks_[attackIndex_].swingSpeed.x, 0.01f);
	ImGui::DragFloat3("SwingRotateSpeed", &kConstAttacks_[attackIndex_].swingRotateSpeed.x, 0.01f);
	ImGui::DragInt("RecoveryTime", reinterpret_cast<int*>(&kConstAttacks_[attackIndex_].recoveryTime));
	ImGui::End();
	
	//武器のImGui
	weapon_->UpdateImGui();
}

void Player::BehaviorRootInitialize()
{

}

void Player::BehaviorRootUpdate()
{
	if (input_->IsControllerConnected())
	{
		//しきい値
		const float threshold = 0.7f;

		//移動フラグ
		bool isMoving = false;

		//移動量
		velocity_ = {
			input_->GetLeftStickX(),
			0.0f,
			input_->GetLeftStickY(),
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtrueにする
		if (Mathf::Length(velocity_) > threshold)
		{
			isMoving = true;
		}

		//スティックによる入力がある場合
		if (isMoving)
		{
			//速さ
			//const float speed = 0.3f;
			const float speed = 0.6f;

			//移動量に速さを反映
			velocity_ = Mathf::Normalize(velocity_) * speed;

			//移動ベクトルをカメラの角度だけ回転する
			Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(camera_->rotation_.y);
			velocity_ = Mathf::TransformNormal(velocity_, rotateMatrix);

			//移動
			worldTransform_.translation_ += velocity_;

			//回転
			Rotate(velocity_);
		}
	}

	//ダッシュ行動に変更
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{
			if (workDash_.coolTime == workDash_.dashCoolTime)
			{
				behaviorRequest_ = Behavior::kDash;
			}
		}
	}

	//ジャンプ行動に変更
	if (input_->IsControllerConnected()) 
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
		{
			behaviorRequest_ = Behavior::kJump;
		}
	}

	//攻撃行動に変更
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
		{
			behaviorRequest_ = Behavior::kAttack;
		}
	}

	//急接近行動に変更
	if (input_->IsControllerConnected())
	{
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_B))
		{
			behaviorRequest_ = Behavior::kRapidApproach;
		}
	}
}

void Player::BehaviorDashInitialize() 
{
	workDash_.dashParameter = 0;
	workDash_.coolTime = 0;
	worldTransform_.quaternion_ = destinationQuaternion_;
}

void Player::BehaviorDashUpdate()
{
	if (input_->IsControllerConnected()) 
	{
		//しきい値
		const float threshold = 0.7f;

		//移動フラグ
		bool isMoving = false;

		//移動量
		Vector3 move = {
			input_->GetLeftStickX(),
			0.0f,
			input_->GetLeftStickY(),
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtrueにする
		if (Mathf::Length(move) > threshold)
		{
			isMoving = true;
		}

		//スティックによる入力がある場合
		if (isMoving) 
		{
			//速さ
			float kSpeed = 1.0f;

			//移動量に速さを反映
			move = Mathf::Normalize(move) *  kSpeed;

			//移動ベクトルをカメラの角度だけ回転する
			Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(camera_->rotation_.y);
			move = Mathf::TransformNormal(move, rotateMatrix);

			//移動
			worldTransform_.translation_ += move;
		}
		//入力がない場合後ろに下がるようにする
		else
		{
			move = { 0.0f,0.0f,-1.0f };

			//移動ベクトルをプレイヤーの角度だけ回転する
			move = Mathf::TransformNormal(move, worldTransform_.matWorld_);

			//移動
			worldTransform_.translation_ += move;
		}
	}

	//規定の時間経過で通常行動に戻る
	const float dashTime = 10;
	if (++workDash_.dashParameter >= dashTime) 
	{
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::BehaviorJumpInitialize() 
{
	const float kJumpFirstSpeed = 0.7f;
	velocity_.x = 0.0f;
	velocity_.y = kJumpFirstSpeed;
	velocity_.z = 0.0f;
}

void Player::BehaviorJumpUpdate() 
{
	if (input_->IsControllerConnected())
	{
		//しきい値
		const float threshold = 0.7f;

		//移動フラグ
		bool isMoving = false;

		//移動量
		Vector3 move = {
			input_->GetLeftStickX(),
			0.0f,
			input_->GetLeftStickY(),
		};

		//スティックの押し込みが遊び範囲を超えていたら移動フラグをtrueにする
		if (Mathf::Length(move) > threshold) 
		{
			isMoving = true;
		}

		//スティックによる入力がある場合
		if (isMoving)
		{
			//速さ
			//const float speed = 0.3f;
			const float speed = 0.6f;

			//移動量に速さを反映
			move = Mathf::Normalize(move) * speed;

			//移動ベクトルをカメラの角度だけ回転する
			Matrix4x4 rotateMatrix = Mathf::MakeRotateYMatrix(camera_->rotation_.y);
			move = Mathf::TransformNormal(move, rotateMatrix);

			//移動
			worldTransform_.translation_ += move;

			//回転
			Rotate(move);
		}
	}

	worldTransform_.translation_ += velocity_;
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	velocity_ += accelerationVector;

	if (worldTransform_.translation_.y <= 1.0f)
	{
		behaviorRequest_ = Behavior::kRoot;
		worldTransform_.translation_.y = 1.0f;
	}
}

void Player::BehaviorAttackInitialize()
{
	//攻撃用の変数を初期化
	workAttack_.translation = { 0.0f,0.0f,0.0f };
	workAttack_.rotation = { 0.0f,0.0f,0.0f };
	workAttack_.attackParameter = 0;
	workAttack_.comboIndex = 0;
	workAttack_.inComboPhase = 0;
	workAttack_.comboNext = false;
}

//コンボ定数表
//アニメーション開始座標 //アニメーション開始角度 振りかぶりの時間 ための時間 攻撃振りの時間 硬直時間 入力受付時間 振りかぶりの移動速度 振りかぶりの回転速度 ための移動速度 ための回転速度,攻撃降りの移動速度 攻撃降りの回転速度
std::array<Player::ConstAttack, Player::ComboNum> Player::kConstAttacks_ = {
	{
		//開始座標             開始角度              振りかぶりの時間 ための時間 攻撃振りの時間 硬直時間 入力受付時間  振りかぶりの移動速度  振りかぶりの回転速度  ための移動速度      ための回転速度       攻撃降りの移動速度   攻撃降りの回転速度
		{{ 0.0f,0.8f,0.0f }, { 0.0f,0.0f,0.0f },  0,            0,       5,          10,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.4f,0.0f,0.0f}},
		{{ 0.0f,0.8f,0.0f }, { 2.8f,0.0f,-1.57f}, 0,            0,       5,          10,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {-0.4f,0.0f,0.0f}},
		{{ 0.0f,0.8f,0.0f }, { 0.0f,0.0f,1.57f }, 0,            0,       20,         10,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.75f,0.0f,0.0f}},
		{{ 0.0f,0.8f,0.0f }, { 2.345f,0.0f,0.0f}, 0,            0,       8,          10,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {-0.26f,0.0f,0.0f}},
	    {{ 0.0f,0.8f,0.0f }, { 0.0f,0.0f,0.0f },  0,            0,       40,         10,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.72f,0.0f,0.0f}},
		{{ 0.0f,0.8f,0.0f }, { 0.0f,0.0f,0.0f },  10,           20,      5,          20,     10,        {0.0f,0.0f,0.0f}, {-0.1f,0.0f,0.0f},{0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.56f,0.0f,0.0f}},

		{{ 0.0f,0.8f,0.0f }, { 0.0f,0.0f,0.0f },  0,            0,       20,         60,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.4f,0.0f,0.0f}},
		{{ 0.0f,0.8f,0.0f }, { 0.0f,0.0f,0.0f },  0,            0,       20,         60,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.4f,0.0f,0.0f}},
		{{ 0.0f,0.8f,0.0f }, { 0.0f,0.0f,0.0f },  0,            0,       20,         60,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.4f,0.0f,0.0f}},
		{{ 0.0f,0.8f,0.0f }, { 0.0f,0.0f,0.0f },  0,            0,       20,         60,     10,        {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f}, {0.4f,0.0f,0.0f}},
	}
};

void Player::BehaviorAttackUpdate() 
{
	//コンボ上限に達していない
	if (workAttack_.comboIndex < ComboNum - 1) 
	{
		if (input_->IsControllerConnected()) 
		{
			//攻撃ボタンをトリガーしたら
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
			{
				//コンボ有効
				workAttack_.comboNext = true;
			}
		}
	}

	//攻撃の合計時間
	uint32_t totalTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime + kConstAttacks_[workAttack_.comboIndex].chargeTime + kConstAttacks_[workAttack_.comboIndex].swingTime + kConstAttacks_[workAttack_.comboIndex].recoveryTime + kConstAttacks_[workAttack_.comboIndex].inputTime;
	//既定の時間経過で通常行動に戻る
	if (++workAttack_.attackParameter >= totalTime)
	{
		//コンボ継続なら次のコンボに進む
		if (workAttack_.comboNext) 
		{
			//コンボ継続フラグをリセット
			workAttack_.comboNext = false;
			workAttack_.attackParameter = 0;
			workAttack_.comboIndex++;
			if (workAttack_.comboIndex >= 0 && workAttack_.comboIndex <= ComboNum - 1)
			{
				workAttack_.translation = kConstAttacks_[workAttack_.comboIndex].startPosition;
				workAttack_.rotation = kConstAttacks_[workAttack_.comboIndex].startRotation;
			}
			weapon_->SetIsAttack(false);
		}
		//コンボ継続でないなら攻撃を終了してルートビヘイビアに戻る
		else 
		{
			behaviorRequest_ = Behavior::kRoot;
			weapon_->SetIsAttack(false);
		}
	}

	switch (workAttack_.comboIndex)
	{
	case 0:
		AttackAnimation();
		break;
	case 1:
		AttackAnimation();
		break;
	case 2:
		AttackAnimation();
		break;
	case 3:
		AttackAnimation();
		break;
	case 4:
		AttackAnimation();
		break;
	case 5:
		AttackAnimation();
		break;
	case 6:
		AttackAnimation();
		break;
	case 7:
		AttackAnimation();
		break;
	case 8:
		AttackAnimation();
		break;
	case 9:
		AttackAnimation();
		break;
	}

	weapon_->SetTranslation(workAttack_.translation);
	weapon_->SetRotation(workAttack_.rotation);
}

void Player::AttackAnimation()
{
	//総合時間
	uint32_t totalTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime + kConstAttacks_[workAttack_.comboIndex].chargeTime + kConstAttacks_[workAttack_.comboIndex].swingTime + kConstAttacks_[workAttack_.comboIndex].recoveryTime + kConstAttacks_[workAttack_.comboIndex].inputTime;

	//現在のコンボの情報を取得
	uint32_t anticipationTime = 0, chargeTime = 0, swingTime = 0, recoveryTime = 0;

	//配列外参照を防ぐ
	if (workAttack_.comboIndex >= 0 && workAttack_.comboIndex <= ComboNum - 1)
	{
		anticipationTime = kConstAttacks_[workAttack_.comboIndex].anticipationTime;
		chargeTime = anticipationTime + kConstAttacks_[workAttack_.comboIndex].chargeTime;
		swingTime = chargeTime + kConstAttacks_[workAttack_.comboIndex].swingTime;
		recoveryTime = swingTime + kConstAttacks_[workAttack_.comboIndex].recoveryTime;
	}

	//振りかぶりの処理
	if (workAttack_.attackParameter < anticipationTime)
	{
		workAttack_.translation += kConstAttacks_[workAttack_.comboIndex].anticipationSpeed;
		workAttack_.rotation += kConstAttacks_[workAttack_.comboIndex].anticipationRotateSpeed;
	}

	//チャージの処理
	if (workAttack_.attackParameter >= anticipationTime && workAttack_.attackParameter < chargeTime)
	{
		workAttack_.translation += kConstAttacks_[workAttack_.comboIndex].chargeSpeed;
		workAttack_.rotation += kConstAttacks_[workAttack_.comboIndex].chargeRotateSpeed;
	}

	//攻撃振りの処理
	if (workAttack_.attackParameter >= chargeTime && workAttack_.attackParameter < swingTime)
	{
		workAttack_.translation += kConstAttacks_[workAttack_.comboIndex].swingSpeed;
		workAttack_.rotation += kConstAttacks_[workAttack_.comboIndex].swingRotateSpeed;
		uint32_t collisionTime = swingTime - chargeTime;
		int collisionInterval = collisionTime / 4;
		if (workAttack_.attackParameter % collisionInterval == 0)
		{
			weapon_->SetIsAttack(true);
		}
		else
		{
			weapon_->SetIsAttack(false);
		}
	}

	//硬直時間
	if (workAttack_.attackParameter >= swingTime && workAttack_.attackParameter < recoveryTime)
	{
		weapon_->SetIsAttack(false);
	}

	//入力受付時間
	if (workAttack_.attackParameter >= recoveryTime && workAttack_.attackParameter < totalTime)
	{
		if (input_->IsControllerConnected())
		{
			//攻撃ボタンをトリガーしたら
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_X))
			{
				//コンボ有効
				workAttack_.attackParameter = totalTime;
			}
		}
	}
}

void Player::BehaviorKnockBackInitialize()
{

}

void Player::BehaviorKnockBackUpdate()
{
	const float kGravityAcceleration = 0.05f;
	Vector3 accelerationVector = { 0.0f,-kGravityAcceleration,0.0f };
	knockBackVelocity_ += accelerationVector;
	worldTransform_.translation_ +=  knockBackVelocity_;

	if (worldTransform_.translation_.y <= 1.0f)
	{
		behaviorRequest_ = Behavior::kRoot;
		worldTransform_.translation_.y = 1.0f;
	}
}

void Player::BehaviorRapidApproachInitialize()
{
	const float kRapidApproachSpeed = 2.0f;
	targetPosition_ = GameObjectManager::GetInstance()->GetGameObject("Boss")->GetTranslation();
	rapidApproachVelocity_ = Mathf::Normalize(targetPosition_ - worldTransform_.translation_) * kRapidApproachSpeed;
	rapidApproachVelocity_.y = 0.0f;
}

void Player::BehaviorRapidApproachUpdate()
{
	worldTransform_.translation_ += rapidApproachVelocity_;
	Rotate(rapidApproachVelocity_);

	if (Mathf::Length(targetPosition_ - worldTransform_.translation_) <= 6.0f)
	{
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::Rotate(const Vector3& v)
{
	Vector3 vector = Mathf::Normalize(v);
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
	destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}