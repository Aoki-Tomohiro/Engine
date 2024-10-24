#include "Application/Src/Game/GameManager.h"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_  HINSTANCE, _In_ LPSTR, _In_ int) {
	GameCore* game = new GameManager();
	game->Run();
	delete game;
	return 0;
}