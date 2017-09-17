#include "DXApp.h"
#include <DirectXColors.h>

DXApp::DXApp(HINSTANCE hInstance) :WinGame(hInstance, "DX11Game_LittleYoung.")
{
}

DXApp::~DXApp()
{
}

bool DXApp::Init()
{
	if (!WinGame::Init())
	{
		return false;
	}
	return true;
}

void DXApp::Update(float dt)
{
}

void DXApp::Render(float dt)
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::CornflowerBlue);

	m_pSwapChain->Present(0, 0);
}