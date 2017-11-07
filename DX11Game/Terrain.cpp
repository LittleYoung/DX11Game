#include "Terrain.h"
#include "Common\GameUtil.h"
#include "Common\GeometryGenerator.h"
#include "Common\GameCamera.h"
#include "Common\DDSTextureLoader.h"
#include "Common\WinGame.h"
#include <fstream>
#include <DirectXPackedVector.h>
#include <algorithm>
using namespace DirectX::PackedVector;

struct Vertex {
	XMFLOAT3 Position;
	XMFLOAT2 TexC;
};

Terrain::Terrain()
	: m_HeightMapWidth(2049)
	, m_HeightMapHeight(2049)
	, m_HeightScale(10.0f)
	, m_CellSpace(0.1f)
{
	m_NumPatchVertRows = (m_HeightMapHeight - 1) / s_CellsPerPatch + 1;
	m_NumPatchVertCols = (m_HeightMapWidth - 1) / s_CellsPerPatch + 1;
}

Terrain::~Terrain()
{
}

bool Terrain::Init()
{
	m_Effects.Init();
	LoadHeightMap();
	SmoothHeightMap();
	LoadHeightMapSRV();
	LoadTextures();
	InitPatchIABuffer();
	InitRState();

	return true;
}

void Terrain::InitRState()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	D3D11_RASTERIZER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.FillMode = D3D11_FILL_WIREFRAME;
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthClipEnable = true;

	HR(device->CreateRasterizerState(&desc, &m_WireRS));
}

void Terrain::LoadTextures()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();
	ID3D11Resource* texResource = nullptr;

	HR(CreateDDSTextureFromFile(device, L"Terrain/blend.dds", &texResource, &m_BlendMapSRV));
	HR(CreateDDSTextureFromFile(device, L"Terrain/array.dds", &texResource, &m_LayerMapArraySRV));
}

void Terrain::InitPatchIABuffer()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	GeometryGenerator::MeshData meshData;
	float width = m_HeightMapWidth*m_CellSpace;
	float height = m_HeightMapHeight*m_CellSpace;
	GeometryGenerator::CreatePlaneMeshData(width, height, m_NumPatchVertCols, m_NumPatchVertRows, meshData);

	std::vector<Vertex> vertices(meshData.Vertices.size());
	for (UINT i = 0; i < meshData.Vertices.size(); i++) {
		vertices[i].Position = meshData.Vertices[i].Position;
		vertices[i].TexC = meshData.Vertices[i].TexC;
	}

	D3D11_BUFFER_DESC vdesc;
	ZeroMemory(&vdesc, sizeof(vdesc));
	vdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vdesc.ByteWidth = meshData.Vertices.size() * sizeof(Vertex);
	vdesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vinit;
	ZeroMemory(&vinit, sizeof(vinit));
	vinit.pSysMem = static_cast<void*>(&vertices[0]);

	HR(device->CreateBuffer(&vdesc, &vinit, &m_vBuffer));

	std::vector<UINT> indices;
	GeometryGenerator::CreatePlaneQuadPatchIB(m_NumPatchVertRows - 1, m_NumPatchVertCols - 1, indices);
	m_IndexCount = indices.size();

	D3D11_BUFFER_DESC idesc;
	ZeroMemory(&idesc, sizeof(idesc));
	idesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	idesc.ByteWidth = sizeof(UINT)*indices.size();
	idesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA iinit;
	ZeroMemory(&iinit, sizeof(iinit));
	iinit.pSysMem = static_cast<void*>(&indices[0]);

	HR(device->CreateBuffer(&idesc, &iinit, &m_iBuffer));

}

void Terrain::LoadHeightMap()
{
	std::ifstream inFile("Terrain/terrain.raw", std::ios_base::binary);
	UINT total = m_HeightMapHeight*m_HeightMapWidth;
	std::vector<unsigned char> heightMapChar(total);
	if (inFile) {
		inFile.read(reinterpret_cast<char*>(&heightMapChar[0]), total);
		inFile.close();
	}

	m_HeightMap.resize(total);
	for (UINT i = 0; i < total; i++) {
		m_HeightMap[i] = (heightMapChar[i] / 255.0f) * m_HeightScale;
	}

}

void Terrain::SmoothHeightMap()
{
	//to do
}

void Terrain::LoadHeightMapSRV()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.Format = DXGI_FORMAT_R16_FLOAT;
	desc.Height = m_HeightMapHeight;
	desc.Width = m_HeightMapWidth;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	std::vector<HALF> hmap(m_HeightMap.size());
	std::transform(m_HeightMap.begin(), m_HeightMap.end(), hmap.begin(), XMConvertFloatToHalf);

	D3D11_SUBRESOURCE_DATA data;
	ZeroMemory(&data, sizeof(data));
	data.pSysMem = static_cast<void*>(&hmap[0]);
	data.SysMemPitch = m_HeightMapWidth * sizeof(HALF);

	ID3D11Texture2D* texture = nullptr;
	HR(device->CreateTexture2D(&desc, &data, &texture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	HR(device->CreateShaderResourceView(texture, &srvDesc, &m_HeightMapSRV));

	SafeRelease(texture);
}

void Terrain::Render(bool ifWire)
{
	GameCamera cam = WinGame::GetInstance()->GetCamera();
	ID3D11DeviceContext* context = WinGame::GetInstance()->GetContext();
	if (ifWire) {
		context->RSSetState(m_WireRS);
	}
	context->IASetInputLayout(m_Effects.GetLayout());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &m_vBuffer, &stride, &offset);
	context->IASetIndexBuffer(m_iBuffer, DXGI_FORMAT_R32_UINT, 0);

	m_Effects.SetEyePosW(cam.GetPos());
	m_Effects.SetViewProj(cam.GetViewProjXM());
	m_Effects.SetHeightSRV(m_HeightMapSRV);
	m_Effects.SetLayerSRV(m_LayerMapArraySRV);
	m_Effects.SetBlendSRV(m_BlendMapSRV);
	m_Effects.Apply();

	context->DrawIndexed(m_IndexCount, 0, 0);

	m_Effects.Clean();
	if (ifWire) {
		context->RSSetState(nullptr);
	}
}

float Terrain::GetHeight(float x, float z)const
{
	float width = (m_HeightMapWidth - 1)*m_CellSpace;
	float height = (m_HeightMapHeight - 1)*m_CellSpace;
	// Transform from terrain local space to "cell" space.
	float c = (x + 0.5f*width) / m_CellSpace;
	float d = (z - 0.5f*height) / -m_CellSpace;

	// Get the row and column we are in.
	int row = (int)floorf(d);
	int col = (int)floorf(c);

	// Grab the heights of the cell we are in.
	// A*--*B
	//  | /|
	//  |/ |
	// C*--*D
	float A = m_HeightMap[row*m_HeightMapWidth + col];
	float B = m_HeightMap[row*m_HeightMapWidth + col + 1];
	float C = m_HeightMap[(row + 1)*m_HeightMapWidth + col];
	float D = m_HeightMap[(row + 1)*m_HeightMapWidth + col + 1];

	// Where we are relative to the cell.
	float s = c - (float)col;
	float t = d - (float)row;

	// If upper triangle ABC.
	if (s + t <= 1.0f)
	{
		float uy = B - A;
		float vy = C - A;
		return A + s*uy + t*vy;
	}
	else // lower triangle DCB.
	{
		float uy = C - D;
		float vy = B - D;
		return D + (1.0f - s)*uy + (1.0f - t)*vy;
	}
}