#include "Player.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Utilities/GameTimer.h"
#include "Application/Src/Object/Player/States/PlayerStateIdle.h"
#include "Application/Src/Object/Player/States/PlayerStateGroundAttack.h"
#include "Application/Src/Object/Enemy/Enemy.h"

void Player::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Stateの初期化
	ChangeState(new PlayerStateIdle());

	//体力バーのスプライトの生成
	TextureManager::Load("HpBar.png");
	spriteHpBar_.reset(Sprite::Create("HpBar.png", { 80.0f,32.0f }));
	spriteHpBar_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });
	TextureManager::Load("HpBarFrame.png");
	spriteHpBarFrame_.reset(Sprite::Create("HpBarFrame.png", { 79.0f,31.0f }));
	spriteHpBarFrame_->SetColor({ 0.0f, 1.0f, 0.0f, 1.0f });

	//被ダメージのスプライトの生成
	damageSprite_.reset(Sprite::Create("white.png", { 0.0f,0.0f }));
	damageSprite_->SetColor(damageSpriteColor_);
	damageSprite_->SetSize({ 1280.0f,720.0f });
}

void Player::Update()
{
	//タイトルシーンにいる場合は移動処理をスキップ
	if (!isInTitleScene_)
	{
		//Stateの更新
		state_->Update();
	}

	//回転処理
	TransformComponent* transformComponent = GetComponent<TransformComponent>();
	transformComponent->worldTransform_.quaternion_ = Mathf::Normalize(Mathf::Slerp(transformComponent->worldTransform_.quaternion_, destinationQuaternion_, 0.4f));

	//Colliderの更新
	AABBCollider* collider = GetComponent<AABBCollider>();
	collider->SetCenter(transformComponent->GetWorldPosition() + colliderOffset_);

	//Gameobjectの更新
	GameObject::Update();

	//HPバーの処理
	hp_ = (hp_ <= 0.0f) ? 0.0f : hp_;
	hpBarSize_ = { (hp_ / kMaxHP) * 480.0f,16.0f };
	spriteHpBar_->SetSize(hpBarSize_);

	//プレイヤーの体力が一定量以下になったらVignetteをかける
	const float hpRatio = 4.0f;
	if (hp_ <= kMaxHP / hpRatio)
	{
		PostEffects::GetInstance()->GetVignette()->SetIsEnable(true);
	}
	else
	{
		PostEffects::GetInstance()->GetVignette()->SetIsEnable(false);
	}

	//無敵時間の処理
	if (isInvincible_)
	{
		invincibleTimer_ += GameTimer::GetDeltaTime();
		if (invincibleTimer_ > invincibleDuration_)
		{
			isInvincible_ = false;
		}

		//ダメージスプライトを徐々に透明にする
		damageSpriteColor_.w = 0.1f * (1.0f - (static_cast<float>(invincibleTimer_) / invincibleDuration_));

		//ダメージスプライトの色を設定
		damageSprite_->SetColor(damageSpriteColor_);
	}

	//ImGui
	ImGui::Begin("Player");
	ImGui::DragFloat3("ColliderOffset", &colliderOffset_.x, 0.1f);
	ImGui::DragFloat("Hp", &hp_);
	ImGui::End();
}

void Player::Draw(const Camera& camera)
{
	//Stateの描画
	state_->Draw(camera);

	//GameObjectの描画
	GameObject::Draw(camera);
}

void Player::DrawUI()
{
	if (isInvincible_)
	{
		damageSprite_->Draw();
	}
	spriteHpBar_->Draw();
	spriteHpBarFrame_->Draw();
}

void Player::OnCollision(GameObject* gameObject)
{
	if (Enemy* enemy = dynamic_cast<Enemy*>(gameObject))
	{
		if (!enemy->GetIsAttack())
		{
			AABBCollider* enemyAABBCollider = enemy->GetComponent<AABBCollider>();
			AABBCollider* playerAABBCollider = GetComponent<AABBCollider>();
			Vector3 overlapAxis = {
				std::min<float>(playerAABBCollider->GetCenter().x + playerAABBCollider->GetMax().x,enemyAABBCollider->GetCenter().x + enemyAABBCollider->GetMax().x) - std::max<float>(playerAABBCollider->GetCenter().x + playerAABBCollider->GetMin().x,enemyAABBCollider->GetCenter().x + enemyAABBCollider->GetMin().x),
				std::min<float>(playerAABBCollider->GetCenter().y + playerAABBCollider->GetMax().y,enemyAABBCollider->GetCenter().y + enemyAABBCollider->GetMax().y) - std::max<float>(playerAABBCollider->GetCenter().y + playerAABBCollider->GetMin().y,enemyAABBCollider->GetCenter().y + enemyAABBCollider->GetMin().y),
				std::min<float>(playerAABBCollider->GetCenter().z + playerAABBCollider->GetMax().z,enemyAABBCollider->GetCenter().z + enemyAABBCollider->GetMax().z) - std::max<float>(playerAABBCollider->GetCenter().z + playerAABBCollider->GetMin().z,enemyAABBCollider->GetCenter().z + enemyAABBCollider->GetMin().z),
			};

			TransformComponent* transformComponent = GetComponent<TransformComponent>();
			TransformComponent* enemyTransformComponent = GetComponent<TransformComponent>();
			Vector3 directionAxis{};
			if (overlapAxis.x < overlapAxis.y && overlapAxis.x < overlapAxis.z) {
				//X軸方向で最小の重なりが発生している場合
				directionAxis.x = (transformComponent->worldTransform_.translation_.x < enemyTransformComponent->worldTransform_.translation_.x) ? -1.0f : 1.0f;
				directionAxis.y = 0.0f;
			}
			else if (overlapAxis.y < overlapAxis.x && overlapAxis.y < overlapAxis.z) {
				//Y軸方向で最小の重なりが発生している場合
				directionAxis.y = (transformComponent->worldTransform_.translation_.y < enemyTransformComponent->worldTransform_.translation_.y) ? -1.0f : 1.0f;
				directionAxis.x = 0.0f;
			}
			else if (overlapAxis.z < overlapAxis.x && overlapAxis.z < overlapAxis.y)
			{
				directionAxis.z = (transformComponent->worldTransform_.translation_.z < enemyTransformComponent->worldTransform_.translation_.z) ? -1.0f : 1.0f;
				directionAxis.x = 0.0f;
				directionAxis.y = 0.0f;
			}

			transformComponent->worldTransform_.translation_ += overlapAxis * directionAxis;
		}
		else
		{
			if (!isInvincible_)
			{
				hp_ -= 10.0f;
				isInvincible_ = true;
				invincibleTimer_ = 0.0f;
				damageSpriteColor_.w = 0.5f;
			}
		}
	}
}

void Player::OnCollisionEnter(GameObject* gameObject)
{

}

void Player::OnCollisionExit(GameObject* gameObject)
{

}

const uint32_t Player::GetComboIndex() const
{
	if (auto state = dynamic_cast<PlayerStateGroundAttack*>(state_.get()))
	{
		return state->GetComboIndex();
	}
	return 0;
}

const uint32_t Player::GetComboNum()
{
	return PlayerStateGroundAttack::kComboNum;
}

void Player::ChangeState(IPlayerState* state)
{
	state->SetPlayer(this);
	state->Initialize();
	state_.reset(state);
}