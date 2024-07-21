#include "PlayerStateJustDodgeAttack.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Player/States/PlayerStateRoot.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void PlayerStateJustDodgeAttack::Initialize()
{
	//アニメーションの設定
	ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
	modelComponent->GetModel()->GetAnimation()->SetIsLoop(false);
	modelComponent->GetModel()->GetAnimation()->SetIsStop(true);
	modelComponent->GetModel()->GetAnimation()->SetAnimationTime(1.28f);
	modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.6f);
	modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0.017");

	//敵の方向に回転させる
	TransformComponent* playerTransformComponent = player_->GetComponent<TransformComponent>();
	TransformComponent* enemyTransformComponent = GameObjectManager::GetInstance()->GetGameObject<Enemy>()->GetComponent<TransformComponent>();
	Vector3 direction = Mathf::Normalize(enemyTransformComponent->GetWorldPosition() - playerTransformComponent->GetWorldPosition());
	direction.y = 0.0f;
	Vector3 cross = Mathf::Cross({ 0.0f,0.0f,1.0f }, direction);
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, direction);
	player_->destinationQuaternion_ = Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot));
	playerTransformComponent->worldTransform_.quaternion_ = player_->destinationQuaternion_;

	//始点座標を決める
	justDodgeAttackWork_.startPosition = playerTransformComponent->GetWorldPosition();

	//終点座標を決める
	justDodgeAttackWork_.targetPosition = enemyTransformComponent->GetWorldPosition() + direction * player_->justDodgeAttackParameters_.targetDistance;
}

void PlayerStateJustDodgeAttack::Update()
{
	//移動が終了していない場合
	if (!justDodgeAttackWork_.isMovementFinished)
	{
		//移動用のタイマーを進める
		justDodgeAttackWork_.moveTimer += GameTimer::GetDeltaTime();

		//現在の進行状況を計算
		float currentTime = justDodgeAttackWork_.moveTimer / player_->justDodgeAttackParameters_.moveDuration;

		//移動処理
		TransformComponent* transformComponent = player_->GetComponent<TransformComponent>();
		transformComponent->worldTransform_.translation_.x = justDodgeAttackWork_.startPosition.x + (justDodgeAttackWork_.targetPosition.x - justDodgeAttackWork_.startPosition.x) * Mathf::EaseOutExpo(currentTime);
		transformComponent->worldTransform_.translation_.y = justDodgeAttackWork_.startPosition.y + (justDodgeAttackWork_.targetPosition.y - justDodgeAttackWork_.startPosition.y) * Mathf::EaseOutExpo(currentTime);
		transformComponent->worldTransform_.translation_.z = justDodgeAttackWork_.startPosition.z + (justDodgeAttackWork_.targetPosition.z - justDodgeAttackWork_.startPosition.z) * Mathf::EaseOutExpo(currentTime);

		//移動時間が一定値を超えたら
		if (justDodgeAttackWork_.moveTimer > player_->justDodgeAttackParameters_.moveDuration)
		{
			//アニメーションを再生させる
			ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
			modelComponent->GetModel()->GetAnimation()->SetIsStop(false);

			//移動終了のフラグを立てる
			justDodgeAttackWork_.isMovementFinished = true;
		}
	}

	//移動が終了していた場合
	if (justDodgeAttackWork_.isMovementFinished)
	{
		//モデルを取得
		ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();

		//ヒットタイマーを進める
		justDodgeAttackWork_.hitTimer += GameTimer::GetDeltaTime();

		//攻撃判定をつける
		Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>();
		if (justDodgeAttackWork_.hitTimer > player_->justDodgeAttackParameters_.hitInterval)
		{
			//ヒット回数が最大数を超えていない場合
			if (justDodgeAttackWork_.hitCount < player_->justDodgeAttackParameters_.maxHitCount)
			{
				weapon->SetIsAttack(true);
				justDodgeAttackWork_.hitCount++;
			}
			justDodgeAttackWork_.hitTimer = 0.0f;
		}
		else
		{
			weapon->SetIsAttack(false);
		}

		//アニメーションが終了していた場合
		if (modelComponent->GetModel()->GetAnimation()->GetIsAnimationEnd())
		{
			//回避攻撃のフラグを初期化
			player_->isJustDodgeAttack_ = false;

			//武器の攻撃判定をなくす
			weapon->SetIsAttack(false);

			//敵の速度をもとに戻す
			Enemy* enemy = GameObjectManager::GetInstance()->GetGameObject<Enemy>();
			enemy->SetTimeScale(1.0f);

			//アニメーションの設定を戻す
			ModelComponent* modelComponent = player_->GetComponent<ModelComponent>();
			modelComponent->GetModel()->GetAnimation()->SetAnimationSpeed(1.0f);
			modelComponent->GetModel()->GetAnimation()->SetIsStop(false);
			modelComponent->GetModel()->GetAnimation()->SetIsLoop(true);

			//通常状態に遷移
			player_->ChangeState(new PlayerStateRoot());
		}
	}
}

void PlayerStateJustDodgeAttack::Draw(const Camera& camera)
{
}

void PlayerStateJustDodgeAttack::OnCollision(GameObject* other)
{
}

void PlayerStateJustDodgeAttack::OnCollisionEnter(GameObject* other)
{
}

void PlayerStateJustDodgeAttack::OnCollisionExit(GameObject* other)
{
}