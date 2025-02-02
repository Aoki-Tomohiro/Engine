/**
 * @file BreakableObject.cpp
 * @brief 破壊オブジェクトを管理するファイル
 * @author 青木智滉
 * @date
 */

#include "BreakableObject.h"
#include "Application/Src/Object/Character/BaseCharacter.h"
#include "Application/Src/Object/Weapon/Weapon.h"

void BreakableObject::Initialize()
{
	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//音声データの読み込み
	audioHandle_ = audio_->LoadAudioFile("RockBreak.mp3");

	//トランスフォームを取得
	transform_ = GetComponent<TransformComponent>();

	//名前を設定
	SetName("BreakableObject");

	//基底クラスの呼び出し
	GameObject::Initialize();
}

void BreakableObject::Update()
{
	//体力が0を下回っていた場合
	if (hp_ <= 0.0f)
	{
		//破壊処理
		HandleDestruction();
	}

	//基底クラスの呼び出し
	GameObject::Update();
}

void BreakableObject::OnCollision(GameObject* gameObject)
{
	//衝突相手がキャラクターの場合
	if (BaseCharacter* character = dynamic_cast<BaseCharacter*>(gameObject))
	{
		//吹き飛ばされていない状態の場合は処理を飛ばす
		if (!character->GetIsKnockedBack()) return;

		//破壊処理
		HandleDestruction();
	}
	//衝突相手が武器の場合
	else if (Weapon* weapon = dynamic_cast<Weapon*>(gameObject))
	{
		hp_ = std::max<float>(0.0f, hp_ - weapon->GetDamage());
	}
}

void BreakableObject::HandleDestruction()
{
	//音声データを再生
	audio_->PlayAudio(audioHandle_, false, 0.1f);

	//パーティクルを生成
	particleEffectEditor_->CreateParticles("RockBreak", transform_->GetWorldPosition(), Mathf::IdentityQuaternion());

	//破壊フラグを立てる
	SetIsDestroy(true);
}