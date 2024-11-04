#include "PlayerStateDash.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/Player/Player.h"

void PlayerStateDash::Initialize()
{
	//インプットのインスタンスを取得
	input_ = Input::GetInstance();

	//オーディオのインスタンスを取得
	audio_ = Audio::GetInstance();

	//音声データの読み込み
	dashAudioHandle_ = audio_->LoadAudioFile("Dash.mp3");

	//ダッシュのフラグを設定
	GetPlayer()->SetActionFlag(Player::ActionFlag::kDashing, true);

	//ダッシュ開始時のアニメーションの再生とアニメーションコントローラーを取得
	SetAnimationControllerAndPlayAnimation(currentAnimation_);
}

void PlayerStateDash::Update()
{
	//アニメーションイベントを実行
	UpdateAnimationState();

	//ダッシュ終了アニメーションへの遷移チェック
	CheckTransitionToDashEndAnimation();

	//速度移動イベントの処理
	HandleVelocityMovement();

	//現在のアニメーションがダッシュ終了かつアニメーションが終了していた場合
	if (character_->GetAnimator()->GetIsAnimationFinished() && currentAnimation_ == "DashEnd")
	{	
		//ダッシュ終了処理
		FinalizeDash();
		//デフォルトの状態に遷移
		HandleStateTransition(false);
	}
}

void PlayerStateDash::OnCollision(GameObject* other)
{
	//衝突処理
	character_->ProcessCollisionImpact(other, true);
}

void PlayerStateDash::InitializeVelocityMovement(const VelocityMovementEvent* velocityMovementEvent, const int32_t animationEventIndex)
{
	//基底クラスの呼び出し
	IPlayerState::InitializeVelocityMovement(velocityMovementEvent, animationEventIndex);

	//敵の座標を取得
	Vector3 enemyPosition = GameObjectManager::GetInstance()->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

	//プレイヤーの座標を取得
	Vector3 playerPosition = character_->GetJointWorldPosition("mixamorig:Hips");

	//差分ベクトルを計算
	Vector3 sub = enemyPosition - playerPosition;

	//Y軸の差分が閾値を超えていない場合は直線に移動させる
	if (std::abs(sub.y) < GetPlayer()->GetDashParameters().verticalAlignmentTolerance)
	{
		processedVelocityDatas_[animationEventIndex].velocity.y = 0.0f;
	}

	//プレイヤーを一時的に非表示にする
	character_->SetIsVisible(false);
	//武器を一時的に非表示にする
	character_->GetWeapon()->SetIsVisible(false);
	//ダッシュのパーティクルを生成
	character_->GetEditorManager()->GetParticleEffectEditor()->CreateParticles("Dash", character_->GetJointWorldPosition("mixamorig:Hips"), character_->GetQuaternion());
	//音声データの再生
	audio_->PlayAudio(dashAudioHandle_, false, 0.2f);
	//ラジアルブラーを有効にする
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(true);
}

void PlayerStateDash::CheckTransitionToDashEndAnimation()
{
	if (currentAnimation_ == "DashStart" && character_->GetAnimator()->GetIsAnimationFinished())
	{
		//先行入力があった場合はダッシュ状態のリセットをして状態を遷移
		if (CheckAndTransitionBufferedAction())
		{
			FinalizeDash();
			return;
		}
		//現在のアニメーションの名前を変更
		currentAnimation_ = "DashEnd";
		//ダッシュ終了時のアニメーションの再生とアニメーションコントローラーを取得
		SetAnimationControllerAndPlayAnimation(currentAnimation_);
	}
}

void PlayerStateDash::FinalizeDash()
{
	//キャラクターを表示
	character_->SetIsVisible(true);
	//武器を表示
	character_->GetWeapon()->SetIsVisible(true);
	//ダッシュのフラグをリセット
	GetPlayer()->SetActionFlag(Player::ActionFlag::kDashing, false);
	//ラジアルブラーを無効化
	PostEffects::GetInstance()->GetRadialBlur()->SetIsEnable(false);
}

void PlayerStateDash::HandleVelocityMovement()
{
	//速度移動イベントが有効の場合
	if (!processedVelocityDatas_.empty() && processedVelocityDatas_[0].isActive)
	{
		//煙のエミッターを更新
		UpdateSmokeEmitters();
	}
	//速度移動イベントが無効の場合はダッシュ終了処理
	else
	{
		FinalizeDash();
	}
}

void PlayerStateDash::UpdateSmokeEmitters()
{
	//煙のエミッター座標を更新
	for (int32_t i = 0; i < 2; ++i)
	{
		if (ParticleEmitter* emitter = character_->GetEditorManager()->GetParticleEffectEditor()->GetEmitter("Smoke", "Smoke" + std::to_string(i + 1)))
		{
			//オフセット値
			Vector3 offset = Mathf::RotateVector(dashParticleOffset_, character_->GetQuaternion()) * static_cast<float>(i);

			//エミッターの座標を設定
			emitter->SetTranslate(character_->GetJointWorldPosition("mixamorig:Hips") + offset);
		}
	}

	//煙のリングのエミッター座標を更新
	if (ParticleEmitter* emitter = character_->GetEditorManager()->GetParticleEffectEditor()->GetEmitter("SmokeRing", "SmokeRing"))
	{
		//エミッターの座標を設定
		emitter->SetTranslate(character_->GetJointWorldPosition("mixamorig:Hips"));
	}
}