#include "WinGame.h"
#include <assert.h>
#include <windowsx.h>
#include <sstream>
#include "GameUtil.h"
#include "MathHelper.h"
#include <DirectXColors.h>

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (WinGame::GetInstance()) {
		return WinGame::GetInstance()->MsgProc(hwnd, msg, wParam, lParam);
	}
	else {
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

WinGame* WinGame::s_Instance = nullptr;

WinGame::WinGame(HINSTANCE hInstance, std::string appTitle)
	:m_ClientWidth(800)
	, m_ClientHeight(600)
	, m_hAppInstance(hInstance)
	, m_AppTitle(appTitle)
	, m_WndStyle(WS_OVERLAPPEDWINDOW)
	, m_hAppWnd(nullptr)
	, m_Enable4XMSAA(false)
	, m_pDevice(nullptr)
	, m_DriverType(D3D_DRIVER_TYPE_HARDWARE)
	, m_pSwapChain(nullptr)
	, m_FeatureLevel()
	, m_pRenderTargetView(nullptr)
	, m_pDepthStencilView(nullptr)
{
	assert(s_Instance == nullptr);
	s_Instance = this;

	m_DirLight.Ambient = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	m_DirLight.Diffuse = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_DirLight.Specular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	m_DirLight.Direction = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);

	m_Cam.SetLens(0.25f*MathHelper::Pi, static_cast<float>(m_ClientWidth) / m_ClientHeight, 1.0f, 1000.0f);
}



bool WinGame::Init()
{
	if (!InitWindow()) { return false; }
	if (!InitDX11()) { return false; }
	return true;
}

int WinGame::Run()
{
	MSG msg = { 0 };

	m_GameTimer.Reset();
	while (WM_QUIT != msg.message) {
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			m_GameTimer.Tick();
			float dt = m_GameTimer.DeltaTime();
			CalculateFrameStats();
			Update(dt);
			Render(dt);
		}
	}
	return static_cast<int>(msg.wParam);
}

LRESULT WinGame::MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
	case WM_LBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
}

void WinGame::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;
	if ((m_GameTimer.GameTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::stringstream outs;
		outs.precision(6);
		outs << m_AppTitle.c_str() << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << mspf << " (ms)";
		SetWindowText(m_hAppWnd, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

bool WinGame::InitWindow()
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(WNDCLASSEX));
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.hInstance = m_hAppInstance;
	wcex.lpfnWndProc = MainWndProc;
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = "DXAPPWNDCLASS";
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex)) {
		OutputDebugString("FAIL TO CREATE WINDOW CLASS\n");
		return false;
	}

	RECT r = { 0, 0, static_cast<long>(m_ClientWidth), static_cast<long>(m_ClientHeight) };
	AdjustWindowRect(&r, m_WndStyle, FALSE);
	UINT width = r.right - r.left;
	UINT height = r.bottom - r.top;

	UINT x = GetSystemMetrics(SM_CXSCREEN) / 2 - width / 2;
	UINT y = GetSystemMetrics(SM_CYSCREEN) / 2 - height / 2;

	m_hAppWnd = CreateWindow("DXAPPWNDCLASS", m_AppTitle.c_str(), m_WndStyle, x, y, width, height, NULL, NULL, m_hAppInstance, NULL);
	if (!m_hAppWnd) {
		OutputDebugString("FAIL TO CREATE WINDOW\n");
		return false;
	}

	ShowWindow(m_hAppWnd, SW_SHOW);

	return true;
}

bool WinGame::InitDX11()
{
	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
	//CREATE DEVICE
	HRESULT hr = D3D11CreateDevice(nullptr, m_DriverType, NULL, createDeviceFlags,
		nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &m_FeatureLevel, &m_pImmediateContext);

	if (FAILED(hr)) {
		OutputDebugString("FAIL TO CREATE D3D11 DEVICE\n");
		return false;
	}

	if (m_FeatureLevel != D3D_FEATURE_LEVEL_11_0) {
		OutputDebugString("FEATURE LEVEL IS NOT 11\n");
		return false;
	}

	//Check	4X MSAA
	UINT m4xMsaa;
	HR(m_pDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &m4xMsaa));
	assert(m4xMsaa > 0);

	DXGI_SWAP_CHAIN_DESC swapDesc;
	ZeroMemory(&swapDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapDesc.BufferDesc.Width = m_ClientWidth;
	swapDesc.BufferDesc.Height = m_ClientHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferCount = 1;
	swapDesc.OutputWindow = m_hAppWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;

	if (m_Enable4XMSAA) {
		swapDesc.SampleDesc.Count = 4;
		swapDesc.SampleDesc.Quality = m4xMsaa - 1;
	}
	else {
		//DISABLE 4XMSAA
		swapDesc.SampleDesc.Count = 1;
		swapDesc.SampleDesc.Quality = 0;
	}
	IDXGIDevice* dxgiDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	IDXGIFactory* dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	HRESULT hrSwap = dxgiFactory->CreateSwapChain(m_pDevice, &swapDesc, &m_pSwapChain);
	if (FAILED(hrSwap)) {
		OutputDebugString("FAIL TO CREATE D3D11 SWAP CHAIN\n");
		return false;
	}
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	ID3D11Texture2D* backBuffer = nullptr;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	m_pDevice->CreateRenderTargetView(backBuffer, nullptr, &m_pRenderTargetView);
	backBuffer->Release();

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilDesc.Width = m_ClientWidth;
	depthStencilDesc.Height = m_ClientHeight;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	if (m_Enable4XMSAA) {
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaa - 1;
	}
	else {
		//NO SMAA
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	ID3D11Texture2D* pDepthStencilBuffer;

	m_pDevice->CreateTexture2D(&depthStencilDesc, 0, &pDepthStencilBuffer);

	m_pDevice->CreateDepthStencilView(pDepthStencilBuffer, 0, &m_pDepthStencilView);

	m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	pDepthStencilBuffer->Release();

	m_ScreenViewPort.TopLeftX = 0;
	m_ScreenViewPort.TopLeftY = 0;
	m_ScreenViewPort.Width = static_cast<float> (m_ClientWidth);
	m_ScreenViewPort.Height = static_cast<float> (m_ClientHeight);
	m_ScreenViewPort.MinDepth = 0;
	m_ScreenViewPort.MaxDepth = 1;

	m_pImmediateContext->RSSetViewports(1, &m_ScreenViewPort);

	return true;
}

WinGame::~WinGame()
{
	if (m_pImmediateContext) {
		m_pImmediateContext->ClearState();
	}
	SafeRelease(m_pDevice);
	SafeRelease(m_pImmediateContext);

	SafeRelease(m_pSwapChain);
	SafeRelease(m_pRenderTargetView);
	SafeRelease(m_pDepthStencilView);
}

void WinGame::Update(float dt)
{
	//
	// Control the camera.
	//
	if (GetAsyncKeyState('W') & 0x8000)
		m_Cam.Walk(10.0f*dt);

	if (GetAsyncKeyState('S') & 0x8000)
		m_Cam.Walk(-10.0f*dt);

	if (GetAsyncKeyState('A') & 0x8000)
		m_Cam.Strafe(-10.0f*dt);

	if (GetAsyncKeyState('D') & 0x8000)
		m_Cam.Strafe(10.0f*dt);

	if (GetAsyncKeyState('Q') & 0x8000)
		m_Cam.Up(10.0f*dt);

	if (GetAsyncKeyState('E') & 0x8000)
		m_Cam.Up(-10.0f*dt);

	m_Cam.UpdateMatrix();
}

void WinGame::Render(float dt)
{
	
}

void WinGame::OnMouseDown(WPARAM btnState, int x, int y)
{
	m_LastMousePos.x = float(x);
	m_LastMousePos.y = float(y);

	SetCapture(m_hAppWnd);
}

void WinGame::OnMouseUp(WPARAM btnState, int x, int y) 
{
	ReleaseCapture();
}

void WinGame::OnMouseMove(WPARAM btnState, int x, int y) 
{
	if ((btnState & MK_LBUTTON) != 0)
	{
		// Make each pixel correspond to a quarter of a degree.
		float dx = XMConvertToRadians(0.25f*static_cast<float>(x - m_LastMousePos.x));
		float dy = XMConvertToRadians(0.25f*static_cast<float>(y - m_LastMousePos.y));

		m_Cam.Pitch(dy);
		m_Cam.RotateY(dx);
	}

	m_LastMousePos.x = float(x);
	m_LastMousePos.y = float(y);
}