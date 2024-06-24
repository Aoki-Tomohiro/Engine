#include "PlayerStateJump.h"
#include "../Player.h"
#include "PlayerStateRoot.h"
#include "Engine/Base/ImGuiManager.h"

void PlayerStateJump::Initialize()
{
	input_ = Input::GetInstance();
}

void PlayerStateJump::Update()
{
	if (input_->IsPressButtonEnter(XINPUT_GAMEPAD_A))
	{
		player_->ChangeState(new PlayerStateRoot());
	}

	ImGui::Begin("PlayerStateJump");
	ImGui::End();
}

void PlayerStateJump::Draw(const Camera& camera)
{
}