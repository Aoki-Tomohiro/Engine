#pragma once
#include "Engine/Framework/Object/GameObject.h"
#include "Engine/Base/ImGuiManager.h"
#include "Engine/Components/Input/Input.h"
#include "Engine/Components/Audio/Audio.h"
#include "Engine/Components/Particle/ParticleManager.h"
#include "Engine/Components/PostEffects/PostEffects.h"
#include "Engine/Math/MathFunction.h"
#include "Engine/Utilities/GameTimer.h"
#include "Engine/Utilities/GlobalVariables.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/CombatAnimationEditor/CombatAnimationEditor.h"
#include "Application/Src/Object/Player/SkillCooldownManager.h"
#include "Application/Src/Object/Player/States/IPlayerState.h"
#include <numbers>

class Player : public GameObject
{
public:
	//ボタンの列挙体
	enum ButtonType
	{
		A, B, X, Y, LB, RB, RT, kMaxButtons,
	};

	//スプライトの構造体
	struct SpriteSettings
	{
		std::unique_ptr<Sprite> buttonSprite = nullptr;
		Vector2 buttonPosition{ 0.0f,0.0f };
		Vector2 buttonScale{ 1.0f,1.0f };
		std::unique_ptr<Sprite> fontSprite = nullptr;
		Vector2 fontPosition{ 0.0f,0.0f };
		Vector2 fontScale{ 1.0f,1.0f };
	};

	//アクションフラグ
	enum class ActionFlag
	{
		kDashing,                      //ダッシュ状態かどうか
		kDashAttackEnabled,            //ダッシュ攻撃状態かどうか
		kAerialAttack,                 //空中攻撃状態かどうか
		kLaunchAttack,                 //打ち上げ攻撃状態かどうか
	};

	//通常状態のパラメーター
	struct RootParameters
	{
		float walkThreshold = 0.3f; //歩きの閾値
		float walkSpeed = 9.0f;     //歩きの移動速度
		float runThreshold = 0.6f;  //走りの閾値
		float runSpeed = 18.0f;     //走りの移動速度
	};

	//ジャンプ状態のパラメーター
	struct JumpParameters
	{
		float jumpFirstSpeed = 35.0f;        //初速度
		float gravityAcceleration = -148.0f; //重力加速度
	};

	//回避用のパラメーター
	struct DodgeParameters
	{
		float dodgeDistance = 10.0f; //回避速度
	};

	//ダッシュ用のパラメーター
	struct DashParameters
	{
		float dashSpeed = 125.0f;                 //ダッシュ速度
		float proximityDistance = 4.0f;           //移動を止める距離
		float verticalAlignmentTolerance = 0.1f;  //プレイヤーと敵のY軸方向の許容誤差
	};

	//攻撃用のパラメーター
	struct AttackParameters
	{
		int32_t comboNum = 4;        //コンボの数
		float attackDistance = 6.0f; //攻撃の補正を掛ける距離
	};

	//打ち上げ攻撃のパラメーター
	struct LaunchAttackParameters : public SkillParameters
	{
		LaunchAttackParameters() : SkillParameters{ 3.0f } {};
	};

	//回転攻撃用のパラメーター
	struct SpinAttackParameters : public SkillParameters
	{
		SpinAttackParameters() : SkillParameters{ 3.0f } {};
		float totalRotation = std::numbers::pi_v<float> * 2.0f * 3.0f;
		float totalDuration = 0.6f;
		float riseHeight = 4.0f;
	};

	//初期化処理
	void Initialize() override;

	//更新処理
	void Update() override;

	//描画処理
	void Draw(const Camera& camera) override;
	void DrawUI() override;

	//衝突処理
	void OnCollision(GameObject* gameObject) override;
	void OnCollisionEnter(GameObject* gameObject) override;
	void OnCollisionExit(GameObject* gameObject) override;

	//状態遷移
	void ChangeState(IPlayerState* newState);

	//プレイヤーの移動
	void Move(const Vector3& velocity);

	//ノックバックを適用
	void ApplyKnockback();

	//プレイヤーの回転
	void Rotate(const Vector3& vector);

	//敵の方向にプレイヤーを向かせる処理
	void LookAtEnemy();

	//アニメーションの補正
	void CorrectAnimationOffset();

	//ダメージの音を再生
	void PlayDamageSound();

	//アニメーション再生
	void PlayAnimation(const std::string& name, float speed, bool loop);

	//アニメーション停止
	void StopAnimation();

	//アニメーション一時停止
	void PauseAnimation();

	//アニメーション再開
	void ResumeAnimation();

	//アニメーションブレンド設定
	void SetIsAnimationBlending(bool isBlending);

	//アニメーション速度設定
	void SetAnimationSpeed(float animationSpeed);

	//アニメーション時間設定
	float GetCurrentAnimationTime();
	void SetCurrentAnimationTime(const float animationTime);
	float GetNextAnimationTime();
	void SetNextAnimationTime(const float animationTime);

	//アニメーションの持続時間を取得
	float GetAnimationDuration();
	float GetNextAnimationDuration();

	//アニメーションの終了チェック
	bool GetIsAnimationFinished();

	//ブレンドの完了チェック
	bool GetIsBlendingCompleted();

	//腰の位置取得
	Vector3 GetHipLocalPosition();
	Vector3 GetHipWorldPosition();

	//位置設定
	Vector3 GetPosition();
	void SetPosition(const Vector3& position);

	//クォータニオンの設定
	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };
	void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

	//アクションフラグの取得・設定
	bool GetActionFlag(const ActionFlag& flag) const
	{
		auto it = flags_.find(flag);
		return it != flags_.end() && it->second;
	}
	void SetActionFlag(const ActionFlag& flag, bool value) { flags_[flag] = value; };

	//体力の取得・設定
	const float GetHP() const { return hp_; };
	void SetHP(const float hp) { hp_ = hp; };

	//スキルのクールダウンが完了しているかを確認する
	const bool GetIsCooldownComplete(const Skill& skill) const;

	//スキルのクールダウンをリセット
	void ResetCooldown(const Skill& skill);

	//ノックバックの設定を設定
	void SetKnockbackSettings(const KnockbackSettings& knockbackSettings) { knockbackSettings_ = knockbackSettings; };

	//アニメーション補正フラグを設定
	void SetIsAnimationCorrectionActive(const float isAnimationCorrectionActive) { isAnimationCorrectionActive_ = isAnimationCorrectionActive; };

	//死亡フラグを取得
	const bool GetIsDead() const { return isDead_; };
	
	//タイトルシーンのフラグを設定
	void SetIsInTitleScene(const bool isInTitleScene) { isInTitleScene_ = isInTitleScene; };

	//ゲームが終了したかどうかのフラグを設定・取得
	const bool GetIsGameFinished() const { return isGameFinished_; };
	void SetIsGameFinished(const bool isGameFinished) { isGameFinished_ = isGameFinished; };
	
	//パーティクルエミッターの追加・削除
	void AddParticleEmitter(const std::string& name, ParticleEmitter* particleEmitter);
	void RemoveParticleEmitter(const std::string& particleName, const std::string& emitterName);

	//加速フィールドの追加・削除
	void AddAccelerationField(const std::string& name, AccelerationField* accelerationField);
	void RemoveAccelerationField(const std::string& particleName, const std::string& accelerationFieldName);

	//ダメージを食らった時のスプライトの色の取得・設定
	const Vector4& GetDamagedSpriteColor() const { return damagedSpriteColor_; };
	void SetDamagedSpriteColor(const Vector4& damagedSpriteColor) { damagedSpriteColor_ = damagedSpriteColor; };

	//パーティクルシステムの取得
	ParticleSystem* GetParticleSystem(const std::string& name) const;

	//カメラの取得・設定
	const Camera* GetCamera() const { return camera_; };
	void SetCamera(const Camera* camera) { camera_ = camera; };

	//ロックオンの取得・設定
	const Lockon* GetLockon() const { return lockon_; };
	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; };

	//CombatAnimationEditorの取得・設定
	const CombatAnimationEditor* GetCombatAnimationEditor() const { return combatAnimationEditor_; };
	void SetCombatAnimationEditor(const CombatAnimationEditor* combatAnimationEditor) { combatAnimationEditor_ = combatAnimationEditor; };

	//各パラメーターの取得
	const RootParameters& GetRootParameters() const { return rootParameters_; };
	const JumpParameters& GetJumpParameters() const { return jumpParameters_; };
	const DodgeParameters& GetDodgeParameters() const { return dodgeParameters_; };
	const DashParameters& GetDashParameters() const { return dashParameters_; };
	const AttackParameters& GetGroundAttackParameters() const { return groundAttackParameters_; };
	const AttackParameters& GetAerialAttackParameters() const { return aerialAttackAttackParameters_; };
	const LaunchAttackParameters& GetLaunchAttackParameters() const { return launchAttackParameters_; };
	const SpinAttackParameters& GetSpinAttackParameters() const { return spinAttackParameters_; };

private:
	//パーティクルの初期化処理
	void InitializeParticleSystems();

	//回転の更新
	void UpdateRotation();

	//コライダーの更新
	void UpdateCollider();

	//移動制限
	void RestrictPlayerMovement(float moveLimit);

	//体力の更新
	void UpdateHP();

	//死亡状態か確認
	void CheckAndTransitionToDeath();

	//デバッグ更新処理
	void DebugUpdate();

	//グローバル変数の適用
	void ApplyGlobalVariables();

	//ImGuiの更新処理
	void UpdateImGui();

private:
	//インプット
	Input* input_ = nullptr;

	//オーディオ
	Audio* audio_ = nullptr;

	//状態
	std::unique_ptr<IPlayerState> state_ = nullptr;

	//スキルクールダウンマネージャー
	std::unique_ptr<SkillCooldownManager> skillCooldownManager_ = nullptr;

	//Quaternion
	Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

	//回転の補間速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//前のフレームのアニメーション後の座標
	Vector3 preAnimationHipPosition_{};

	//コライダーのオフセット値
	Vector3 colliderOffset_{};

	//アクションフラグ
	std::unordered_map<ActionFlag, bool> flags_{};

	//ノックバックの設定
	KnockbackSettings knockbackSettings_{};

	//パーティクル
	std::map<std::string, ParticleSystem*> particleSystems_{};

	//カメラ
	const Camera* camera_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

	//CombatAnimationEditor
	const CombatAnimationEditor* combatAnimationEditor_ = nullptr;

	//体力
	const float kMaxHP = 100.0f;
	float hp_ = kMaxHP;

	//死亡フラグ
	bool isDead_ = false;

	//ボタンのスプライト
	std::vector<SpriteSettings> buttonSprites_{};
	std::vector<SpriteSettings> RTbuttonSprites_{};
	const Vector2 kMaxCoolDownSpriteScale = { 28.0f,4.0f };
	std::vector<std::unique_ptr<Sprite>> skillCoolDownSprites_{};

	//ダメージエフェクトのスプライト
	std::unique_ptr<Sprite> damagedSprite_ = nullptr;
	Vector4 damagedSpriteColor_ = { 1.0f,0.0f,0.0f,0.0f };

	//体力のスプライト
	std::unique_ptr<Sprite> hpSprite_ = nullptr;
	std::unique_ptr<Sprite> hpFrameSprite_ = nullptr;
	Vector2 hpSpritePosition_ = { 80.0f,32.0f };
	Vector2 hpSpriteSize_{ 480.0f,16.0f };
	Vector2 hpFrameSpritePosition_ = { 79.0f,31.0f };

	//オーディオハンドル
	uint32_t damageAudioHandle_ = 0;

	//通常状態のパラメーター
	RootParameters rootParameters_{};

	//ジャンプ状態のパラメーター
	JumpParameters jumpParameters_{};

	//回避状態のパラメーター
	DodgeParameters dodgeParameters_{};

	//ダッシュ用のパラメーター
	DashParameters dashParameters_{};

	//地上攻撃用のパラメーター
	AttackParameters groundAttackParameters_{ .comboNum = 4,.attackDistance = 3.0f,};

	//空中攻撃用のパラメーター
	AttackParameters aerialAttackAttackParameters_{ .comboNum = 3,.attackDistance = 3.0f,};

	//打ち上げ攻撃のパラメーター
	LaunchAttackParameters launchAttackParameters_{};

	//回転攻撃のパラメーター
	SpinAttackParameters spinAttackParameters_{};

	//アニメーションの座標のずれを補正したかどうか
	bool isAnimationCorrectionActive_ = false;

	//ゲームが終了したかどうか
	bool isGameFinished_ = false;

	//タイトルシーンなのか
	bool isInTitleScene_ = false;

	//デバッグ用のフラグ
	bool isDebug_ = false;

	//アニメーション時間
	float animationTime_ = 0.0f;

	//現在のアニメーション
	std::string currentAnimationName_ = "Idle";

	//アニメーション一覧
	std::vector<std::string> animationName_ = { 
		{"Idle"}, {"Walk1"}, {"Walk2"}, {"Walk3"}, {"Walk4"}, {"Run1"}, {"Run2"}, {"Run3"}, {"Run4"}, {"Jump1"}, {"Jump2"}, {"Dodge1"}, {"Dodge2"}, {"Dodge3"}, {"Dodge4"},
		{"DashStart"}, {"DashEnd"}, {"Falling"}, {"GroundAttack1"}, {"GroundAttack2"}, {"GroundAttack3"}, {"GroundAttack4"}, {"AerialAttack1"}, {"AerialAttack2"}, {"AerialAttack3"},
		{"LaunchAttack"},{"SpinAttack"},
	};
};

