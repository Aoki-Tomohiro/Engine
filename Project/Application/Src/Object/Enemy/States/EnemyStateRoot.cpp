#include "EnemyStateRoot.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/ModelComponent.h"
#include "Engine/Utilities/RandomGenerator.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Enemy/Enemy.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"
#include "Application/Src/Object/Enemy/States/EnemyStateMissile.h"
#include "Application/Src/Object/Enemy/States/EnemyStateLaserAttack.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDash.h"
#include "Application/Src/Object/Enemy/States/EnemyStateJumpAttack.h"
#include "Application/Src/Object/Enemy/States/EnemyStateComboAttack.h"
#include "Application/Src/Object/Enemy/States/EnemyStateDead.h"
#include "Application/Src/Object/Player/Player.h"

int EnemyStateRoot::preAction_ = kMaxCloseRangeActions;

void EnemyStateRoot::Initialize()
{
	//次の行動の間隔を決める
	currentActionInterval_ = RandomGenerator::GetRandomFloat(enemy_->rootParameters_.minActionInterval_, enemy_->rootParameters_.maxActionInterval_);

	//アニメーションの初期化
	if (ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>())
	{
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0");
	}
}

void EnemyStateRoot::Update()
{
	//デバッグのフラグが立っているとき
	ModelComponent* modelComponent = enemy_->GetComponent<ModelComponent>();

	//動いている状態でなければ処理を飛ばす
	if (!enemy_->isMove_)
	{
		return;
	}

	//ImGui
	ImGui::Begin("Enemy");
	if (enemy_->isDebug_)
	{
		//アニメーションを再生するかを決める
		modelComponent->GetModel()->GetAnimation()->SetIsStop(enemy_->isAnimationStop_);
		//アニメーションが再生されていなければアニメーションの時間を設定
		if (enemy_->isAnimationStop_)
		{
			//アニメーションが再生されていなければアニメーションの時間を設定
			modelComponent->GetModel()->GetAnimation()->SetAnimationTime(enemy_->animationTime_);
		}
		const char* items[] = { "TackleAttack", "JumpAttack", "ComboAttack", "LaserAttack", "Root" };
		if (ImGui::Combo("State", &currentItem_, items, IM_ARRAYSIZE(items)))
		{
			switch (currentItem_)
			{
			case kTackle:
				enemy_->ChangeState(new EnemyStateTackle());
				break;
			case kJumpAttack:
				enemy_->ChangeState(new EnemyStateJumpAttack());
				break;
			case kComboAttack:
				enemy_->ChangeState(new EnemyStateComboAttack());
				break;
			case kLaserAttack:
				enemy_->ChangeState(new EnemyStateLaserAttack());
				break;
			}
		}
		ImGui::End();
		return;
	}
	ImGui::End();

	//プレイヤーのトランスフォームを取得
	TransformComponent* playerTransformComponent = GameObjectManager::GetInstance()->GetGameObject<Player>()->GetComponent<TransformComponent>();

	//差分ベクトルを計算
	TransformComponent* enemyTransformComponent = enemy_->GetComponent<TransformComponent>();
	Vector3 sub = playerTransformComponent->GetWorldPosition() - enemyTransformComponent->GetWorldPosition();
	sub.y = 0.0f;

	//距離を計算
	float distance = Mathf::Length(sub);

	//プレイヤーとの距離が一定以上離れていたら
	if (distance >= enemy_->rootParameters_.stopDistance)
	{
		//速度を計算して移動させる
		enemy_->velocity = Mathf::Normalize(sub) * enemy_->rootParameters_.moveSpeed;
		enemyTransformComponent->worldTransform_.translation_ += enemy_->velocity * GameTimer::GetDeltaTime() * enemy_->timeScale_;

		//アニメーションを歩きにする
		modelComponent->SetAnimationName("Armature.001|mixamo.com|Layer0");
	}
	else
	{
		//速度を0にする
		enemy_->velocity = { 0.0f,0.0f,0.0f };

		//アニメーションを通常状態にする
		modelComponent->SetAnimationName("Armature|mixamo.com|Layer0");
	}

	//回転処理
	sub = Mathf::Normalize(sub);
	Vector3 cross = Mathf::Normalize(Mathf::Cross({ 0.0f,0.0f,1.0f }, sub));
	float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, sub);
	enemy_->destinationQuaternion_ = Mathf::Normalize(Mathf::MakeRotateAxisAngleQuaternion(cross, std::acos(dot)));

	//HPが0になったら
	if (enemy_->hp_ <= 0.0f)
	{
		//死亡状態にする
		enemy_->ChangeState(new EnemyStateDead());
		return;
	}

	//アクションタイマーを進める
	actionTimer_ += GameTimer::GetDeltaTime() * enemy_->timeScale_;

	//アクションタイマーが一定間隔を超えていたら
	if (actionTimer_ > currentActionInterval_)
	{
		//プレイヤーとの距離が近距離行動の距離より近かった場合
		if (distance < enemy_->rootParameters_.closeRangeDistance)
		{
			int nextAction = kTackle;
			while (true)
			{
				nextAction = RandomGenerator::GetRandomInt(0, kMaxCloseRangeActions);
				if (nextAction != preAction_)
				{
					break;
				}
			}
			switch (nextAction)
			{
			case kTackle:
				enemy_->ChangeState(new EnemyStateTackle());
				break;
			case kJumpAttack:
				enemy_->ChangeState(new EnemyStateJumpAttack());
				break;
			case kComboAttack:
				enemy_->ChangeState(new EnemyStateComboAttack());
				break;
			case kLaserAttack:
				enemy_->ChangeState(new EnemyStateLaserAttack());
				break;
			}
			//次のアクション状態を保存
			preAction_ = nextAction;
		}
		else
		{
			int nextAction = RandomGenerator::GetRandomInt(0, kMaxCloseRangeActions);
			switch (nextAction)
			{
			case kDash:
				enemy_->ChangeState(new EnemyStateDash());
				break;
			}
		}
	}
}

void EnemyStateRoot::Draw(const Camera& camera)
{
}