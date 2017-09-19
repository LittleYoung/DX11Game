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
	m_Cam.SetPosition(XMFLOAT3(0, 10, 0));

	m_Rain.Init();
	return true;
}

void DXApp::Update(float dt)
{
	WinGame::Update(dt);
}

void DXApp::Render(float dt)
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::CornflowerBlue);
	m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	m_Rain.Render();

	m_pSwapChain->Present(0, 0);
}