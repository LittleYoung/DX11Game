#include "Sky.h"
#include "Common\DDSTextureLoader.h"
#include "Common\GeometryGenerator.h"
#include "Common\GameUtil.h"
#include "Common\GameCamera.h"
#include "Common\WinGame.h"

Sky::Sky()
{
}


Sky::~Sky()
{
}

bool Sky::Init()
{
	m_SkyEffects.Init();
	InitMesh();
	InitState();
	return true;
}

void Sky::InitState()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	D3D11_DEPTH_STENCIL_DESC lessEqualDesc;
	lessEqualDesc.DepthEnable = true;
	lessEqualDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	lessEqualDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	lessEqualDesc.StencilEnable = false;

	HR(device->CreateDepthStencilState(&lessEqualDesc, &m_LessEqualDSS));

	D3D11_RASTERIZER_DESC rasterDesc;
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.CullMode = D3D11_CULL_NONE;
	rasterDesc.DepthClipEnable = true;
	HR(device->CreateRasterizerState(&rasterDesc, &m_NoCullRS));
}

void Sky::InitMesh()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	ID3D11Resource* texResource = nullptr;

	HR(CreateDDSTextureFromFile(device, L"Textures\\snowcube1024.dds", &texResource, &m_CubeMapSRV));

	GeometryGenerator::MeshData meshData;
	GeometryGenerator::CreateSphereMeshData(1000, 3, meshData);

	std::vector<XMFLOAT3> vertices(meshData.Vertices.size());
	for (UINT i = 0; i < vertices.size(); i++) {
		vertices[i] = meshData.Vertices[i].Position;
	}

	D3D11_BUFFER_DESC vdesc;
	ZeroMemory(&vdesc, sizeof(vdesc));
	vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vdesc.ByteWidth = sizeof(XMFLOAT3)*vertices.size();
	vdesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vinit;
	ZeroMemory(&vinit, sizeof(vinit));
	vinit.pSysMem = reinterpret_cast<void*>(&vertices[0]);
	HR(device->CreateBuffer(&vdesc, &vinit, &m_SkyVB));

	m_SkyIndexCount = meshData.Indices.size();
	D3D11_BUFFER_DESC idesc;
	ZeroMemory(&idesc, sizeof(idesc));
	idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	idesc.ByteWidth = sizeof(UINT)*meshData.Indices.size();
	idesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA iinit;
	ZeroMemory(&iinit, sizeof(iinit));
	iinit.pSysMem = reinterpret_cast<void*>(&meshData.Indices[0]);
	HR(device->CreateBuffer(&idesc, &iinit, &m_SkyIB));
}

void Sky::Render()
{
	ID3D11DeviceContext* context = WinGame::GetInstance()->GetContext();
	GameCamera cam = WinGame::GetInstance()->GetCamera();

	context->IASetInputLayout(m_SkyEffects.GetLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->RSSetState(m_NoCullRS);
	context->OMSetDepthStencilState(m_LessEqualDSS, 0);

	UINT stride = sizeof(XMFLOAT3);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_SkyVB, &stride, &offset);
	context->IASetIndexBuffer(m_SkyIB, DXGI_FORMAT_R32_UINT, 0);

	XMFLOAT3 eyePos = cam.GetPos();
	XMMATRIX T = XMMatrixTranslation(eyePos.x, eyePos.y, eyePos.z);


	XMMATRIX WVP = XMMatrixMultiply(T, cam.GetViewProjXM());

	m_SkyEffects.SetCubeMap(m_CubeMapSRV);
	m_SkyEffects.SetWorldViewProj(WVP);
	m_SkyEffects.Apply();
	context->DrawIndexed(m_SkyIndexCount, 0, 0);

	context->RSSetState(nullptr);
	context->OMSetDepthStencilState(nullptr, 0);
}