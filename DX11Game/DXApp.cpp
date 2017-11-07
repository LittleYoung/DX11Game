#include "DXApp.h"
#include <DirectXColors.h>

DXApp::DXApp(HINSTANCE hInstance) :WinGame(hInstance, "DX11Game_LittleYoung.")
{
	m_DirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_DirLight.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	m_DirLight.Direction = XMFLOAT3(0.57735f, -0.07735f, 0.57735f);
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
	m_Terrain.Init();
	m_Sky.Init();
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

	if (GetAsyncKeyState('1') & 0x8000) {
		m_Terrain.Render(true);
	} else {
		m_Terrain.Render(false);
	}

	m_Sky.Render();
	m_pSwapChain->Present(0, 0);
}