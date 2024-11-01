#include "PlayerStateDash.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateFalling.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateAttack.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateStun.h"
#include "Application/Src/Object/Weapon/Weapon.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void PlayerStateDash::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//音声データの読み込み
	dashAudioHandle_ = audio_->LoadAudioFile("Dash.mp3");

	//ダッシュのフラグを設定
	player_->SetActionFlag(Player::ActionFlag::kDashing, true);

	//アニメーションの状態の取得
	animationState_ = player_->GetCombatAnimationEditor()->GetAnimationState("Dash");

	//ダッシュ開始時のアニメーションを再生
	player_->GetAnimator()->PlayAnimation("DashStart", 1.0f, false);
}

void PlayerStateDash::Update()
{
	//前のアニメーションのフェーズを更新
	prePhaseIndex_ = currentPhaseIndex_;

	//アニメーションの時間を進める
	animationTime_ += GameTimer::GetDeltaTime();

	//アニメーションのフェーズの更新
	UpdateAnimationPhase();

	//現在のフェーズに応じた処理
	switch (currentPhaseIndex_)
	{
	case kCharge:
		break;
	case kDash:
		//ダッシュ状態の更新
		DashUpdate();
		break;
	case kRecovery:
		//硬直状態の更新
		RecoveryUpdate();
		break;
	}
}

void PlayerStateDash::OnCollision(GameObject* other)
{
	//ダッシュの状態をリセット
	ResetDashFlags();

	//ラジアルブラーを切る
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);

	//衝突処理
	player_->ProcessCollisionImpact(other, true);
}

void PlayerStateDash::HandleLockon()
{
	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetJointWorldPosition("mixamorig:Hips");

	//敵とプレイヤーの方向を計算
	Vector3 sub = Mathf::Normalize(enemyPosition - playerPosition);

	//Y軸の差分が閾値を超えていない場合は直線に移動させる
	sub.y = std::abs(sub.y) < player_->GetDashParameters().verticalAlignmentTolerance ? 0.0f : sub.y;

	//計算した方向と速度でダッシュベクトルを設定
	velocity_ = sub * animationState_.phases[currentPhaseIndex_].attackSettings.moveSpeed;;

	//プレイヤーを敵の方向に向ける
	player_->Rotate({ sub.x, 0.0f, sub.z });
}

void PlayerStateDash::HandleNonLockon()
{
	//プレイヤーの前方にダッシュ
	velocity_ = Mathf::RotateVector({ 0.0f, 0.0f, 1.0f }, player_->GetDestinationQuaternion());
	velocity_ = Mathf::Normalize(velocity_) * animationState_.phases[currentPhaseIndex_].attackSettings.moveSpeed;
}

void PlayerStateDash::UpdateAnimationPhase()
{
	//アニメーションのフェーズの更新
	if (currentPhaseIndex_ < animationState_.phases.size() - 1 && animationTime_ > animationState_.phases[currentPhaseIndex_].duration)
	{
		//現在のフェーズを更新
		currentPhaseIndex_++;

		//ダッシュフェーズになったら速度を計算
		if (animationState_.phases[currentPhaseIndex_].name == "Dash")
		{
			//ロックオン中の場合敵の方向に向かってダッシュ
			if (player_->GetLockon()->ExistTarget())
			{
				HandleLockon();
			}
			//ロックオンしていない場合前方に向かってダッシュ
			else
			{
				HandleNonLockon();
			}
		}
	}
}

void PlayerStateDash::DashUpdate()
{
	//フェーズが切り替わった場合
	if (prePhaseIndex_ != currentPhaseIndex_)
	{
		//プレイヤーを一時的に非表示にする
		player_->SetIsVisible(false);

		//武器を一時的に非表示にする
		Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("PlayerWeapon");
		weapon->SetIsVisible(false);

		//ダッシュのパーティクルを生成
		player_->GetParticleEffectManager()->CreateParticles("Dash", player_->GetJointWorldPosition("mixamorig:Hips"), player_->GetDestinationQuaternion());

		//ラジアルブラーを有効にする
		PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(true);

		//音声データの再生
		audio_->PlayAudio(dashAudioHandle_, false, 0.2f);
	}

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

	//プレイヤーの座標を取得
	Vector3 playerPosition = player_->GetJointWorldPosition("mixamorig:Hips");

	//敵と一定距離近づいていた場合
	if (Mathf::Length(enemyPosition - playerPosition) < player_->GetDashParameters().proximityDistance)
	{
		//速度を0にする
		velocity_ = { 0.0f,0.0f,0.0f };
	}

	//移動処理
	player_->Move(velocity_);

	//地面に埋まらないようにする
	Vector3 position = player_->GetPosition();
	position.y = std::max<float>(position.y, 0.0f);
	player_->SetPosition(position);

	//エミッターの座標を更新
	UpdateEmitterPosition();

	//Xボタンを押していた場合ダッシュ攻撃のフラグを立てる
	if (player_->IsTriggered(Player::ButtonType::X))
	{
		player_->SetActionFlag(Player::ActionFlag::kDashAttackEnabled, true);
	}
}

void PlayerStateDash::RecoveryUpdate()
{
	//フェーズが変わったとき
	if (prePhaseIndex_ != currentPhaseIndex_)
	{
		HandlePhaseChange();
	}
	//アニメーションが終了した時
	else if (player_->GetAnimator()->GetIsAnimationFinished())
	{
		HandleAnimationFinish();
	}
}

void PlayerStateDash::UpdateEmitterPosition()
{
	//煙のエミッター座標を更新
	for (int32_t i = 0; i < 2; ++i)
	{	
		if (ParticleEmitter* emitter = player_->GetParticleEffectManager()->GetEmitter("Smoke", "Smoke" + std::to_string(i + 1)))
		{
			//オフセット値
			Vector3 offset = Mathf::RotateVector(dashParticleOffset_, player_->GetDestinationQuaternion()) * static_cast<float>(i);

			//エミッターの座標を設定
			emitter->SetTranslate(player_->GetJointWorldPosition("mixamorig:Hips") + offset);
		}
	}

	//煙のリングのエミッター座標を更新
	if (ParticleEmitter* emitter = player_->GetParticleEffectManager()->GetEmitter("SmokeRing", "SmokeRing" ))
	{
		//エミッターの座標を設定
		emitter->SetTranslate(player_->GetJointWorldPosition("mixamorig:Hips"));
	}
}

void PlayerStateDash::HandlePhaseChange()
{
	//ダッシュ終了時のアニメーションを再生
	player_->GetAnimator()->StopAnimation();
	player_->GetAnimator()->PlayAnimation("DashEnd", 1.0f, false);

	//ダッシュの状態をリセット
	ResetDashFlags();

	//ラジアルブラーを切る
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);

	//ダッシュ攻撃のフラグが立っている場合は攻撃状態に遷移
	if (player_->GetActionFlag(Player::ActionFlag::kDashAttackEnabled))
	{
		player_->ChangeState(new PlayerStateAttack());
	}
}

void PlayerStateDash::ResetDashFlags()
{
	//プレイヤーを再表示
	player_->SetIsVisible(true);
	//武器を再表示
	Weapon* weapon = GameObjectManager::GetInstance()->GetGameObject<Weapon>("PlayerWeapon");
	weapon->SetIsVisible(true);
	//ダッシュのフラグを解除
	player_->SetActionFlag(Player::ActionFlag::kDashing, false);
}

void PlayerStateDash::HandleAnimationFinish()
{
	//プレイヤーが空中にいる場合
	if (player_->GetPosition().y > 0.0f)
	{
		//落下状態に遷移
		player_->ChangeState(new PlayerStateFalling());
	}
	else
	{
		//通常状態に戻す
		player_->ChangeState(new PlayerStateRoot());
	}
}