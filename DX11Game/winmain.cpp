#include "DXApp.h"

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WinGame* app = new DXApp(hInstance);
	if (!app->Init())
	{
		return false;
	}

	return app->Run();
}