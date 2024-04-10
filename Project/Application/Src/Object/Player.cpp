#include "Player.h"

void Player::Initialize(Model* model) {
	//モデルの初期化
	assert(model);
	model_ = model;
	//ワールドトランスフォームの初期化
	worldTransform_.Initialize();
}

void Player::Update() {
	//移動限界座標
	const float kMoveLimitX = 20.0f;
	worldTransform_.translation_.x = max(worldTransform_.translation_.x, -kMoveLimitX);
	worldTransform_.translation_.x = min(worldTransform_.translation_.x, +kMoveLimitX);

	//ワールドトランスフォームの更新
	worldTransform_.UpdateMatrixFromEuler();
}

void Player::Draw(const Camera& camera) {
	//モデルの描画
	model_->Draw(worldTransform_, camera);
}

void Player::MoveLeft() {
	const float kMoveSpeed = 0.4f;
	worldTransform_.translation_.x -= kMoveSpeed;
}

void Player::MoveRight() {
	const float kMoveSpeed = 0.4f;
	worldTransform_.translation_.x += kMoveSpeed;
}