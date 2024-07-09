#include "LockOn.h"
#include "Engine/Base/Application.h"
#include "Engine/Base/TextureManager.h"
#include "Engine/Components/Component/TransformComponent.h"
#include "Engine/Math/MathFunction.h"

void LockOn::Initialize()
{
	//インスタンスを取得
	input_ = Input::GetInstance();

	//スプライトの生成
	TextureManager::Load("Reticle.png");
	lockOnMark_.reset(Sprite::Create("Reticle.png", { 0.0f,0.0f }));
	lockOnMark_->SetAnchorPoint({ 0.5f,0.5f });
}

void LockOn::Update(GameObject* gameObject, const Camera& camera)
{
	//ロックオン状態なら
	if (target_)
	{
		//ロックオンマークの座標計算
		SetLockOnMarkPosition(camera);

		//ロックオン解除処理
		if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER))
		{
			//ロックオンを外す
			target_ = nullptr;
		}
		else if (InRange(camera))
		{
			//ロックオンを外す
			target_ = nullptr;
		}
	}
	else
	{
		//ロックオン対象の検索
		if (input_->IsControllerConnected())
		{
			if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_LEFT_SHOULDER))
			{
				SearchLockOnTarget(gameObject, camera);
			}
		}
	}
}

void LockOn::Draw()
{
	if (target_)
	{
		lockOnMark_->Draw();
	}
}

Vector3 LockOn::GetTargetPosition() const
{
	if (target_)
	{
		TransformComponent* transformComponent = target_->GetComponent<TransformComponent>();
		return transformComponent->GetWorldPosition();
	}
	return Vector3();
}

bool LockOn::InRange(const Camera& camera)
{
	//敵のロックオン座標取得
	TransformComponent* transformComponent = target_->GetComponent<TransformComponent>();
	Vector3 positionWorld = transformComponent->GetWorldPosition();
	//ワールド→ビュー座標変換
	Vector3 positionView = Mathf::Transform(positionWorld, camera.matView_);

	//距離条件チェック
	if (minDistance_ <= positionView.z && positionView.z <= maxDistance_)
	{
		////カメラ前方との角度を計算
		//float arcTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, positionView);
		float length = Mathf::Length(positionView);
		float angle = std::acos(dot / length);

		//角度条件チェック(コーンに収まっているか)
		if (std::abs(angle) <= angleRange_) {
			return false;
		}
	}

	return true;
}

void LockOn::SearchLockOnTarget(GameObject* gameObject, const Camera& camera)
{
	//敵のロックオン座標取得
	TransformComponent* transformComponent = gameObject->GetComponent<TransformComponent>();
	Vector3 positionWorld = transformComponent->GetWorldPosition();
	//ワールド→ビュー座標変換
	Vector3 positionView = Mathf::Transform(positionWorld, camera.matView_);

	//距離条件チェック
	if (minDistance_ <= positionView.z && positionView.z <= maxDistance_) {
		//カメラ前方との角度を計算
		//float arcTangent = std::atan2(std::sqrt(positionView.x * positionView.x + positionView.y * positionView.y), positionView.z);
		float dot = Mathf::Dot({ 0.0f,0.0f,1.0f }, positionView);
		float length = Mathf::Length(positionView);
		float angle = std::acos(dot / length);

		//角度条件チェック(コーンに収まっているか)
		if (std::abs(angle) <= angleRange_) {
			target_ = nullptr;
			target_ = gameObject;
			SetLockOnMarkPosition(camera);
		}
	}
}

Vector2 LockOn::WorldToScreenPosition(const Vector3& worldPosition, const Camera& camera)
{
	// ビューポート行列
	Matrix4x4 matViewport = Mathf::MakeViewportMatrix(0, 0, Application::kClientWidth, Application::kClientHeight, 0, 1);
	// ビュー行列とプロジェクション行列、ビューポート行列を合成する
	Matrix4x4 matViewProjectionViewport = camera.matView_ * camera.matProjection_ * matViewport;
	//ワールド座標からスクリーン座標に変換
	Vector3 positionScreen = Mathf::Transform(worldPosition, matViewProjectionViewport);
	//Vector2に変換して返す
	return Vector2(positionScreen.x, positionScreen.y);
}

void LockOn::SetLockOnMarkPosition(const Camera& camera)
{
	TransformComponent* transformComponent = target_->GetComponent<TransformComponent>();
	Vector3 positionWorld = transformComponent->GetWorldPosition() + targetOffset_;
	Vector2 positionScreenV2 = WorldToScreenPosition(positionWorld, camera);
	lockOnMark_->SetPosition(positionScreenV2);
}