#include "Enemy.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Components/Collision/AABBCollider.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Player/Player.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Enemy/States/EnemyStateTackle.h"

void Enemy::Initialize()
{
	//GameObjectの初期化
	GameObject::Initialize();

	//Stateの初期化
	ChangeState(new EnemyStateIdle());

	//体力バーのスプライトの生成
	TextureManager::Load("HpBar.png");
	spriteHpBar_.reset(Sprite::Create("HpBar.png", { 720.0f,32.0f }));
	spriteHpBar_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });

	//体力バーのフレームのスプライトの生成
	TextureManager::Load("HpBarFrame.png");
	spriteHpBarFrame_.reset(Sprite::Create("HpBarFrame.png", { 719.0f,31.0f }));
	spriteHpBarFrame_->SetColor({ 1.0f, 0.1f, 0.0f, 1.0f });
}

void Enemy::Update()
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

	//GameObjectの更新
	GameObject::Update();

	//HPバーの処理
	hpBarSize_ = { (hp_ / kMaxHP) * 480.0f,16.0f };
	spriteHpBar_->SetSize(hpBarSize_);

	//ImGui
	ImGui::Begin("Enemy");
	ImGui::DragFloat3("ColliderOffset", &colliderOffset_.x, 0.1f);
	ImGui::End();
}

void Enemy::Draw(const Camera& camera)
{
	//GameObjectの描画
	GameObject::Draw(camera);
}

void Enemy::DrawUI()
{
	spriteHpBar_->Draw();
	spriteHpBarFrame_->Draw();
}

void Enemy::OnCollision(GameObject* gameObject)
{

}

void Enemy::OnCollisionEnter(GameObject* gameObject)
{
	if (Weapon* weapon = dynamic_cast<Weapon*>(gameObject))
	{
		const Player* player = GameObjectManager::GetInstance()->GetGameObject<Player>();
		switch (player->GetComboIndex())
		{
		case 0:
			hp_ -= 12.0f;
			break;
		case 1:
			hp_ -= 12.0f;
			break;
		case 2:
			hp_ -= 8.0f;
			break;
		case 3:
			hp_ -= 15.0f;
			break;
		}
	}
}

void Enemy::OnCollisionExit(GameObject* gameObject)
{

}

void Enemy::ChangeState(IEnemyState* state)
{
	state->SetEnemy(this);
	state->Initialize();
	state_.reset(state);
}