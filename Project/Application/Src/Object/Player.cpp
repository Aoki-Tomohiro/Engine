#include "Player.h"

void Player::Initialize()
{
	input_ = Input::GetInstance();

	worldTransform_.Initialize();
}

void Player::Update()
{
	if (input_->IsControllerConnected())
	{
		bool isMoveing = false;

		const float threshold = 0.7f;

		Vector3 velocity = {
			input_->GetLeftStickX(),
			0.0f,
			input_->GetLeftStickY(),
		};

		if (Mathf::Length(velocity) > threshold)
		{
			isMoveing = true;
		}

		if (isMoveing)
		{
			const float kMoveSpeed = 0.1f;
			Vector3 normalizeVector = Mathf::Normalize(velocity);
			velocity = normalizeVector * kMoveSpeed;
			worldTransform_.translation_ += velocity;

			Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, normalizeVector));
			float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, normalizeVector);
			destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
		}
	}

	worldTransform_.quaternion_ = Mathf::Slerp(worldTransform_.quaternion_, destinationQuaternion_, 0.4f);
	worldTransform_.UpdateMatrixFromQuaternion();
}

void Player::Draw(const Camera& camera)
{
	if (model_)
	{
		model_->Draw(worldTransform_, camera);
	}
}

void Player::DrawUI()
{

}