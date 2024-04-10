#include "GameTitleScene.h"

void GameTitleScene::Initialize()
{
	renderer_ = Renderer::GetInstance();

	input_ = Input::GetInstance();

	audio_ = Audio::GetInstance();
}

void GameTitleScene::Finalize()
{

}

void GameTitleScene::Update() 
{
	Quaternion rotation0 = Mathf::MakeRotateAxisAngleQuaternion({ 0.71f,0.71f,0.0f }, 0.3f);
	Quaternion rotation1 = Mathf::MakeRotateAxisAngleQuaternion({ 0.71f,0.0f,0.71f }, 3.141592f);

	Quaternion interpolate0 = Mathf::Slerp(rotation0, rotation1, 0.0f);
	Quaternion interpolate1 = Mathf::Slerp(rotation0, rotation1, 0.3f);
	Quaternion interpolate2 = Mathf::Slerp(rotation0, rotation1, 0.5f);
	Quaternion interpolate3 = Mathf::Slerp(rotation0, rotation1, 0.7f);
	Quaternion interpolate4 = Mathf::Slerp(rotation0, rotation1, 1.0f);

	ImGui::Begin("TitleScene");
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate0, Slerp(q0,q1,0.0f)",
		interpolate0.x, interpolate0.y, interpolate0.z, interpolate0.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate1, Slerp(q0,q1,0.3f)",
		interpolate1.x, interpolate1.y, interpolate1.z, interpolate1.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate2, Slerp(q0,q1,0.5f)",
		interpolate2.x, interpolate2.y, interpolate2.z, interpolate2.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate3, Slerp(q0,q1,0.7f)",
		interpolate3.x, interpolate3.y, interpolate3.z, interpolate3.w);
	ImGui::Text("%1.2f %1.2f %1.2f %1.2f : interpolate4, Slerp(q0,q1,1.0f)",
		interpolate4.x, interpolate4.y, interpolate4.z, interpolate4.w);
	ImGui::End();
}

void GameTitleScene::Draw()
{
#pragma region 背景スプライト描画
	//背景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//背景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion

	//深度バッファをクリア
	renderer_->ClearDepthBuffer();

#pragma region 3Dオブジェクト描画
	//3Dオブジェクト描画
	renderer_->Render();
#pragma endregion

#pragma region パーティクル描画
	//パーティクル描画前処理
	renderer_->PreDrawParticles();

	//パーティクル描画後処理
	renderer_->PostDrawParticles();
#pragma endregion
}

void GameTitleScene::DrawUI() 
{
#pragma region 前景スプライト描画
	//前景スプライト描画前処理
	renderer_->PreDrawSprites(kBlendModeNormal);

	//前景スプライト描画後処理
	renderer_->PostDrawSprites();
#pragma endregion
}