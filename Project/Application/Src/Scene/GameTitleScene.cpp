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
	Vector3 from0 = Mathf::Normalize(Vector3{ 1.0f,0.7f,0.5f });
	Vector3 to0 = { -from0.x,-from0.y,-from0.z };
	Vector3 from1 = Mathf::Normalize(Vector3{ -0.6f, 0.9f, 0.2f });
	Vector3 to1 = Mathf::Normalize(Vector3{ 0.4f,0.7f,-0.5f });
	Matrix4x4 rotateMatrix0 = Mathf::DirectionToDirection(
		Mathf::Normalize(Vector3{ 1.0f,0.0f,0.0f }), Mathf::Normalize(Vector3{ -1.0f,0.0f,0.0f }));
	Matrix4x4 rotateMatrix1 = Mathf::DirectionToDirection(from0, to0);
	Matrix4x4 rotateMatrix2 = Mathf::DirectionToDirection(from1, to1);

	ImGui::Begin("TitleScene");
	ImGui::Text("rotateMatrix0");
	ImGui::Text("%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f",
		rotateMatrix0.m[0][0], rotateMatrix0.m[0][1], rotateMatrix0.m[0][2], rotateMatrix0.m[0][3],
		rotateMatrix0.m[1][0], rotateMatrix0.m[1][1], rotateMatrix0.m[1][2], rotateMatrix0.m[1][3],
		rotateMatrix0.m[2][0], rotateMatrix0.m[2][1], rotateMatrix0.m[2][2], rotateMatrix0.m[2][3],
		rotateMatrix0.m[3][0], rotateMatrix0.m[3][1], rotateMatrix0.m[3][2], rotateMatrix0.m[3][3]);
	ImGui::Text("rotateMatrix1");
	ImGui::Text("%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f",
		rotateMatrix1.m[0][0], rotateMatrix1.m[0][1], rotateMatrix1.m[0][2], rotateMatrix1.m[0][3],
		rotateMatrix1.m[1][0], rotateMatrix1.m[1][1], rotateMatrix1.m[1][2], rotateMatrix1.m[1][3],
		rotateMatrix1.m[2][0], rotateMatrix1.m[2][1], rotateMatrix1.m[2][2], rotateMatrix1.m[2][3],
		rotateMatrix1.m[3][0], rotateMatrix1.m[3][1], rotateMatrix1.m[3][2], rotateMatrix1.m[3][3]);
	ImGui::Text("rotateMatrix2");
	ImGui::Text("%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f\n%1.3f %1.3f %1.3f %1.3f",
		rotateMatrix2.m[0][0], rotateMatrix2.m[0][1], rotateMatrix2.m[0][2], rotateMatrix2.m[0][3],
		rotateMatrix2.m[1][0], rotateMatrix2.m[1][1], rotateMatrix2.m[1][2], rotateMatrix2.m[1][3],
		rotateMatrix2.m[2][0], rotateMatrix2.m[2][1], rotateMatrix2.m[2][2], rotateMatrix2.m[2][3],
		rotateMatrix2.m[3][0], rotateMatrix2.m[3][1], rotateMatrix2.m[3][2], rotateMatrix2.m[3][3]);
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