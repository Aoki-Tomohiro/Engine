#include "PlayerStateDash.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateDash::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//ダッシュのフラグを設定
	player_->SetActionFlag(Player::ActionFlag::kDashing, true);

	//状態の取得
	animationState_ = player_->GetAnimationStateManager()->GetAnimationState("Dash");

	//ダッシュ開始時のアニメーションを再生
	player_->PlayAnimation("DashStart", 1.0f, false);

	//ロックオン中の場合
	if (player_->GetLockon()->ExistTarget())
	{
		//敵の座標を取得
		Vector3 enemyPosition = GameObjectManager::GetInstance()->GetMutableGameObject<Enemy>("")->GetHipWorldPosition();

		//プレイヤーの座標
		Vector3 playerPosition = player_->GetHipWorldPosition();

		//差分ベクトルを計算
		Vector3 sub = Mathf::Normalize(enemyPosition - playerPosition);

		//水平方向に移動させるので速度のY成分を0にする
		sub.y = 0.0f;

		//速度を計算
		velocity_ = sub * player_->GetDashParameters().dashSpeed;

		//回転処理
		player_->Rotate(sub);
	}
	//ロックオン中ではない場合
	else
	{
		//速度を計算
		velocity_ = Mathf::RotateVector({ 0.0f,0.0f,-1.0f }, player_->GetDestinationQuaternion());

		//速度を掛ける
		velocity_ = Mathf::Normalize(velocity_) * player_->GetDashParameters().dashSpeed;
	}

	//テクスチャの読み込み
	TextureManager::Load("smoke_01.png");

	//パーティクルの作成
	particleSystem_ = ParticleManager::Create("Dash");
	particleSystem_->SetTexture("smoke_01.png");

	//加速フィールドを作成
	AccelerationField* accelerationField = new AccelerationField();
	accelerationField->Initialize("Dash", 100.0f);
	accelerationField->SetAcceleration({ 0.0f,0.8f,0.0f });
	accelerationField->SetMin({ -200.0f,-200.0f,-200.0f });
	accelerationField->SetMax({ 200.0f,200.0f,200.0f });
	particleSystem_->AddAccelerationField(accelerationField);
}

void PlayerStateDash::Update()
{
	for()
}