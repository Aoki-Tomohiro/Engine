#include "Player.h"
#include "Project/Src/Object/LockOn/LockOn.h"

void Player::Initialize()
{
	worldTransform_.Initialize();
	worldTransform_.translation_.y = 1.0f;
	worldTransform_.translation_.z = -20.0f;

	//入力クラスのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//スプライトの生成
	spriteHpBar_.reset(Sprite::Create("HpBar.png", { 80.0f,32.0f }));
	spriteHpBar_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	spriteHpBarFrame_.reset(Sprite::Create("HpBarFrame.png", { 79.0f,31.0f }));
	spriteHpBarFrame_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });

	//衝突属性を設定
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
}

void Player::Update()
{
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

	//HPバーの処理
	hpBarSize_ = { (hp_ / kMaxHP) * 480.0f,16.0f };
	spriteHpBar_->SetSize(hpBarSize_);
}

void Player::Draw(const Camera& camera)
{
	model_->Draw(worldTransform_, camera);
}

void Player::DrawUI()
{
	spriteHpBar_->Draw();
	spriteHpBarFrame_->Draw();
}

void Player::OnCollision(Collider* collider)
{

}

const Vector3 Player::GetWorldPosition() const
{
	Vector3 pos{};
	pos.x = worldTransform_.matWorld_.m[3][0];
	pos.y = worldTransform_.matWorld_.m[3][1];
	pos.z = worldTransform_.matWorld_.m[3][2];
	return pos;
}

void Player::BehaviorRootInitialize()
{
	gravity_ = { 0.0f,0.0f,0.0f };
}

void Player::BehaviorRootUpdate()
{
	//移動処理
	const float speed = 0.6f;
	Move(speed);

	//地面にいなかったら落下する
	if (worldTransform_.translation_.y >= 1.0f)
	{
		const float kGravityAcceleration = 0.05f;
		gravity_.y -= kGravityAcceleration;
		worldTransform_.translation_ += gravity_;

		if (worldTransform_.translation_.y < 1.0f)
		{
			worldTransform_.translation_.y = 1.0f;
		}
	}
}

void Player::Move(const float speed)
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
}

void Player::Rotate(const Vector3& v)
{
	Vector3 vector = Mathf::Normalize(v);
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, vector));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, vector);
	destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
}