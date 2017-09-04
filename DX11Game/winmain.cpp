#include "WinGame.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WinGame* app = new WinGame(hInstance,"hello...");
	if (!app->Init())
	{
		return 1;
	}

	return app->Run();
}