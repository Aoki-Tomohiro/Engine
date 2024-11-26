#pragma once
#include "Engine/3D/Camera/Camera.h"
#include "Engine/3D/Transform/WorldTransform.h"
#include "Engine/Math/MathFunction.h"
#include "Application/Src/Object/Lockon/Lockon.h"
#include "Application/Src/Object/Camera/CameraShake.h"
#include "Application/Src/Object/Camera/States/ICameraState.h"
#include "Application/Src/Object/Editors/CameraAnimationEditor/CameraAnimationEditor.h"

/// <summary>
/// カメラ制御クラス
/// </summary>
class CameraController
{
public:
	//通常カメラ時のパラメーター
	struct FollowCameraParameters
	{
		Vector3 offset = { 0.0f, 1.0f, -10.0f }; // オフセット値
		float rotationRangeMin = 1.0f;           // 最小回転角度
		float rotationRangeMax = 2.4f;           // 最大回転角度
		float rotationSpeedX = 0.04f;            // X軸の回転速度
		float rotationSpeedY = 0.08f;            // Y軸の回転速度
	};

	//ロックオンカメラ時のパラメーター
	struct LockonCameraParameters
	{
		Vector3 offset = { 0.0f, 1.0f, -10.0f }; //オフセット値
		float maxDistance = 0.2f;                //追従対象とロックオン対象が近接していると判定する最大距離
	};

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 状態を遷移
	/// </summary>
	/// <param name="state">新しい状態</param>
	void ChangeState(ICameraState* state);

	/// <summary>
	/// アニメーションを再生
	/// </summary>
	/// <param name="animationName">アニメーションの名前</param>
	/// <param name="animationSpeed">アニメーションの速度</param>
	/// <param name="syncWithCharacterAnimation">キャラクターのアニメーションと同期するかどうか</param>
	void PlayAnimation(const std::string& animationName, const float animationSpeed, const bool syncWithCharacterAnimation);

	/// <summary>
	/// カメラシェイクの開始
	/// </summary>
	/// <param name="intensity">カメラシェイクの強さ</param>
	/// <param name="duration">カメラシェイクの持続時間</param>
	void StartCameraShake(const Vector3& intensity, const float duration);

	//カメラ座標を取得・設定
	const Vector3& GetCameraPosition() const { return camera_.translation_; };
	void SetCameraPosition(const Vector3& position) { camera_.translation_ = position; };

	//補間クォータニオンを取得・設定
	const Quaternion& GetDestinationQuaternion() const { return destinationQuaternion_; };
	void SetDestinationQuaternion(const Quaternion& destinationQuaternion) { destinationQuaternion_ = destinationQuaternion; };

	//追従対象からのオフセットを取得・設定
	const Vector3& GetOffset() const { return offset_; };
	void SetOffset(const Vector3& offset) { offset_ = offset; };

	//Fovを取得・設定
	const float GetFov() const { return camera_.fov_; };
	void SetFov(const float fov) { camera_.fov_ = fov; };

	//ロックオンを取得・設定
	const Lockon* GetLockon() const { return lockon_; };
	void SetLockon(const Lockon* lockOn) { lockon_ = lockOn; };

	//追従対象を取得・設定
	const TransformComponent* GetTarget() const { return target_; };
	void SetTarget(const TransformComponent* target) { target_ = target; };

	//カメラアニメーションエディターを取得・設定
	CameraAnimationEditor* GetCameraAnimationEditor() const { return cameraAnimationEditor_; };
	void SetCameraAnimationEditor(CameraAnimationEditor* cameraAnimationEditor) { cameraAnimationEditor_ = cameraAnimationEditor; };

	//追従対象の残像座標を取得
	const Vector3& GetInterTarget() const { return interTarget_; };

	//カメラを取得
	const Camera& GetCamera() const { return camera_; };

	//各パラメーターを取得
	const FollowCameraParameters& GetFollowCameraParameters() const { return followCameraParameters_; };
	const LockonCameraParameters& GetLockonCameraParameters() const { return lockonCameraParameters_; };

private:
	/// <summary>
	/// 残像座標の更新
	/// </summary>
	void UpdateInterTargetPosition();

	/// <summary>
	/// カメラ座標の更新
	/// </summary>
	void UpdateCameraPosition();

	/// <summary>
	/// カメラの回転を更新
	/// </summary>
	void UpdateCameraRotation();

	/// <summary>
	/// オフセット値を計算
	/// </summary>
	/// <returns>オフセット値</returns>
	const Vector3 CalculateOffset() const;

	/// <summary>
	/// Rayが地面と交差するかどうか
	/// </summary>
	/// <param name="rayOrigin">Rayの開始位置</param>
    /// <param name="rayDirection">Rayの進行方向</param>
    /// <param name="maxDistance">Rayの最大距離</param>
    /// <returns>交差しているかどうか</returns>
	const bool CheckRayIntersectsGround(const Vector3& rayOrigin, const Vector3& rayDirection, float maxDistance);

private:
	//カメラの状態
	std::unique_ptr<ICameraState> state_ = nullptr;

	//カメラシェイク
	std::unique_ptr<CameraShake> cameraShake_ = nullptr;

	//カメラ
	Camera camera_{};

	//追従対象
	const TransformComponent* target_ = nullptr;

	//ロックオン
	const Lockon* lockon_ = nullptr;

	//追従対象からのオフセット
	Vector3 offset_{};

	//追従対象の残像座標
	Vector3 interTarget_{};

	//追従対象のオフセット
	Vector3 targetOffset_ = { 0.0f,2.0f,0.0f };

	//追従対象の補間速度
	float targetInterpolationSpeed_ = 0.2f;

	//クォータニオン
	Quaternion destinationQuaternion_ = Mathf::IdentityQuaternion();

	//クォータニオンの補間速度
	float quaternionInterpolationSpeed_ = 0.4f;

	//地面との交差点
	Vector3 intersectionPoint_{};

	//カメラアニメーションエディター
	CameraAnimationEditor* cameraAnimationEditor_ = nullptr;

	//各パラメーター
	FollowCameraParameters followCameraParameters_{};
	LockonCameraParameters lockonCameraParameters_{};
};

