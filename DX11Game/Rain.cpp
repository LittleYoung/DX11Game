#include "Rain.h"
#include "Common\MathHelper.h"
#include "Common\GameUtil.h"
#include "Common\GameCamera.h"
#include "Common\DDSTextureLoader.h"
#include "Common\WinGame.h"
#include "Common\GameCamera.h"

namespace Vertex {
	struct Particle
	{
		XMFLOAT3 InitialPos;
		XMFLOAT3 InitialVel;
		float Age;
		unsigned int Type;
	};
}

Rain::Rain()
	: m_MaxParticles(10000)
	, m_FirstRun(true)
{
}

Rain::~Rain()
{
}

bool Rain::Init()
{
	InitTexSRV();
	InitRandomTex();
	InitVB();
	InitState();
	m_Effects.Init();

	return true;
}

void Rain::InitState()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	D3D11_DEPTH_STENCIL_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.DepthEnable = FALSE;

	HR(device->CreateDepthStencilState(&desc, &m_NoDepthDSS));
}

void Rain::InitTexSRV()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	ID3D11Resource* texResource = nullptr;
	HR(CreateDDSTextureFromFile(device, L"Textures/raindrop.dds", &texResource, &m_RainTexSRV));
}

void Rain::InitRandomTex()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	XMFLOAT4 random[1024];
	for (int i = 0; i < 1024; i++) {
		random[i].x = MathHelper::RandF(-1, 1);
		random[i].y = MathHelper::RandF(-1, 1);
		random[i].z = MathHelper::RandF(-1, 1);
		random[i].w = MathHelper::RandF(-1, 1);
	}

	D3D11_TEXTURE1D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	desc.MipLevels = 1;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.Width = 1024;

	D3D11_SUBRESOURCE_DATA initdata;
	ZeroMemory(&initdata, sizeof(initdata));
	initdata.pSysMem = random;
	initdata.SysMemPitch = 1024 * sizeof(XMFLOAT4);

	ID3D11Texture1D* texture = nullptr;
	HR(device->CreateTexture1D(&desc, &initdata, &texture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Texture1D.MipLevels = desc.MipLevels;
	srvDesc.Texture1D.MostDetailedMip = 0;
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;

	HR(device->CreateShaderResourceView(texture, &srvDesc, &m_RandomMapSRV));
	SafeRelease(texture);
}

void Rain::InitVB()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(Vertex::Particle);
	desc.Usage = D3D11_USAGE_DEFAULT;

	Vertex::Particle p;
	ZeroMemory(&p, sizeof(p));

	D3D11_SUBRESOURCE_DATA vinit;
	ZeroMemory(&vinit, sizeof(vinit));
	vinit.pSysMem = &p;

	HR(device->CreateBuffer(&desc, &vinit, &m_InitVB));

	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	desc.ByteWidth = sizeof(Vertex::Particle)*m_MaxParticles;

	HR(device->CreateBuffer(&desc, 0, &m_DrawVB));
	HR(device->CreateBuffer(&desc, 0, &m_StreamVB));

	m_Effects.SetSOBufferStride(sizeof(Vertex::Particle));
}

void Rain::Render()
{
	GameCamera cam = WinGame::GetInstance()->GetCamera();

	m_Effects.SetDeltaTime(WinGame::GetInstance()->GetDeltaTime());
	m_Effects.SetEyePosW(cam.GetPos());
	m_Effects.SetGameTime(WinGame::GetInstance()->GetGameTime());
	m_Effects.SetRandomSRV(m_RandomMapSRV);
	m_Effects.SetTexSRV(m_RainTexSRV);
	m_Effects.SetViewProj(cam.GetViewProjXM());

	ID3D11DeviceContext* context = WinGame::GetInstance()->GetContext();

	UINT stride = sizeof(Vertex::Particle);
	UINT offset = 0;

	context->IASetInputLayout(m_Effects.GetLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	if (m_FirstRun) {
		context->IASetVertexBuffers(0, 1, &m_InitVB, &stride, &offset);
	} else {
		context->IASetVertexBuffers(0, 1, &m_DrawVB, &stride, &offset);
	}
	context->SOSetTargets(1, &m_StreamVB, &offset);
	context->OMSetDepthStencilState(m_NoDepthDSS, 0);

	m_Effects.Clean();
	m_Effects.ApplyStream();
	if (m_FirstRun) {
		context->Draw(1, 0);
		m_FirstRun = false;
	} else {
		context->DrawAuto();
	}

	ID3D11Buffer* tmpbuffer[1] = { nullptr };
	context->SOSetTargets(1, tmpbuffer, &offset);
	context->OMSetDepthStencilState(nullptr, 0);

	std::swap(m_DrawVB, m_StreamVB);

	context->IASetVertexBuffers(0, 1, &m_DrawVB, &stride, &offset);
	m_Effects.ApplyDraw();
	context->DrawAuto();

	m_Effects.Clean();
}