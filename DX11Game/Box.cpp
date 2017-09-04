#include "Box.h"
#include <DirectXColors.h>

Box::Box(HINSTANCE hInstance) :WinGame(hInstance, "hello box.")
{
}

Box::~Box()
{
}

bool Box::Init()
{
	if (!WinGame::Init())
	{
		return false;
	}
	return true;
}

void Box::Update(float dt)
{
}

void Box::Render(float dt)
{
	m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, DirectX::Colors::CornflowerBlue);

	m_pSwapChain->Present(0, 0);
}