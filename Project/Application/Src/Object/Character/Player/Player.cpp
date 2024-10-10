#include "Player.h"
#include "Engine/Framework/Object/GameObjectManager.h"
#include "Application/Src/Object/Character/States/PlayerStates/PlayerStateRoot.h"
#include "Application/Src/Object/Character/Enemy/Enemy.h"

void Player::Initialize()
{
	//基底クラスの初期化
	ICharacter::Initialize();

	//状態の初期化
	ChangeState(new PlayerStateRoot());
}

void Player::Update()
{
	//タイトルシーンにいない場合
	if (!isInTitleScene_)
	{
		//状態の更新
		state_->Update();
	}

	//基底クラスの更新
	ICharacter::Update();
}

void Player::DrawUI()
{
	//基底クラスの描画
	ICharacter::DrawUI();
}

void Player::ChangeState(IPlayerState* newState)
{
	//新しい状態の設定
	newState->SetPlayer(this);

	//新しい状態の初期化
	newState->Initialize();

	//現在の状態を新しい状態に更新
	state_.reset(newState);
}

void Player::LookAtEnemy()
{
	//プレイヤーの腰のジョイントのワールド座標を取得
	Vector3 playerPosition = GetJointWorldPosition("mixamorig:Hips");

	//敵の座標を取得
	Vector3 enemyPosition = gameObjectManager_->GetGameObject<Enemy>("Enemy")->GetJointWorldPosition("mixamorig:Hips");

	//敵の方向へのベクトルを計算
	Vector3 rotateVector = Mathf::Normalize(enemyPosition - playerPosition);

	//Y軸にだけ回転させたいのでY成分を0にする
	rotateVector.y = 0.0f;

	//回転処理
	Rotate(Mathf::Normalize(rotateVector));
}

void Player::InitializeAnimator()
{
	//基底クラスの呼び出し
	ICharacter::InitializeAnimator();

	//アニメーションを追加
	animator_->AddAnimation("Idle", AnimationManager::Create("Player/Animations/Idle.gltf"));
	animator_->AddAnimation("Walk1", AnimationManager::Create("Player/Animations/Walk1.gltf"));
	animator_->AddAnimation("Walk2", AnimationManager::Create("Player/Animations/Walk2.gltf"));
	animator_->AddAnimation("Walk3", AnimationManager::Create("Player/Animations/Walk3.gltf"));
	animator_->AddAnimation("Walk4", AnimationManager::Create("Player/Animations/Walk4.gltf"));
	animator_->AddAnimation("Run1", AnimationManager::Create("Player/Animations/Run1.gltf"));
	animator_->AddAnimation("Run2", AnimationManager::Create("Player/Animations/Run2.gltf"));
	animator_->AddAnimation("Run3", AnimationManager::Create("Player/Animations/Run3.gltf"));
	animator_->AddAnimation("Run4", AnimationManager::Create("Player/Animations/Run4.gltf"));
	animator_->AddAnimation("Jump1", AnimationManager::Create("Player/Animations/Jump1.gltf"));
	animator_->AddAnimation("Jump2", AnimationManager::Create("Player/Animations/Jump2.gltf"));
	animator_->AddAnimation("DodgeForward", AnimationManager::Create("Player/Animations/DodgeForward.gltf"));
	animator_->AddAnimation("DodgeBackward", AnimationManager::Create("Player/Animations/DodgeBackward.gltf"));
	animator_->AddAnimation("DashStart", AnimationManager::Create("Player/Animations/DashStart.gltf"));
	animator_->AddAnimation("DashEnd", AnimationManager::Create("Player/Animations/DashEnd.gltf"));
	animator_->AddAnimation("Falling", AnimationManager::Create("Player/Animations/Falling.gltf"));
	animator_->AddAnimation("GroundAttack1", AnimationManager::Create("Player/Animations/GroundAttack1.gltf"));
	animator_->AddAnimation("GroundAttack2", AnimationManager::Create("Player/Animations/GroundAttack2.gltf"));
	animator_->AddAnimation("GroundAttack3", AnimationManager::Create("Player/Animations/GroundAttack3.gltf"));
	animator_->AddAnimation("GroundAttack4", AnimationManager::Create("Player/Animations/GroundAttack4.gltf"));
	animator_->AddAnimation("AerialAttack1", AnimationManager::Create("Player/Animations/AerialAttack1.gltf"));
	animator_->AddAnimation("AerialAttack2", AnimationManager::Create("Player/Animations/AerialAttack2.gltf"));
	animator_->AddAnimation("AerialAttack3", AnimationManager::Create("Player/Animations/AerialAttack3.gltf"));
	animator_->AddAnimation("LaunchAttack", AnimationManager::Create("Player/Animations/LaunchAttack.gltf"));
	animator_->AddAnimation("SpinAttack", AnimationManager::Create("Player/Animations/SpinAttack.gltf"));
	animator_->AddAnimation("FallingAttack", AnimationManager::Create("Player/Animations/FallingAttack.gltf"));
	animator_->AddAnimation("Impact", AnimationManager::Create("Player/Animations/Impact.gltf"));
	animator_->AddAnimation("Death", AnimationManager::Create("Player/Animations/Death.gltf"));
	animator_->AddAnimation("Casting", AnimationManager::Create("Player/Animations/Casting.gltf"));
	animator_->AddAnimation("MagicAttack", AnimationManager::Create("Player/Animations/MagicAttack.gltf"));
}

void Player::InitializeUISprites()
{
	//テクスチャの名前を設定
	hpTextureNames_ = { {
		{"barBack_horizontalLeft.png", "barBack_horizontalMid.png", "barBack_horizontalRight.png"},
		{"barRed_horizontalLeft.png", "barRed_horizontalMid.png", "barRed_horizontalRight.png"},
		}
	};

	//スプライトの座標を設定
	hpBarSegmentPositions_ = { {
		{ {	{71.0f, 40.0f}, {80.0f, 40.0f}, {560.0f, 40.0f},}},
		{ { {71.0f, 40.0f}, {80.0f, 40.0f}, {560.0f, 40.0f},}},
		}
	};

	//基底クラスの呼び出し
	ICharacter::InitializeUISprites();
}