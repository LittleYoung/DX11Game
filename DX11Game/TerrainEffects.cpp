#include "TerrainEffects.h"
#include "Common\GameUtil.h"
#include "Common\WinGame.h"
#include <d3dcompiler.h>

TerrainEffects::TerrainEffects()
{
}

TerrainEffects::~TerrainEffects()
{
}

bool TerrainEffects::Init()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	ID3DBlob *vsBlob, *psBlob, *hsBlob, *dsBlob;
	HR(D3DReadFileToBlob(L"Terrain_vs.cso", &vsBlob));
	HR(D3DReadFileToBlob(L"Terrain_ps.cso", &psBlob));
	HR(D3DReadFileToBlob(L"Terrain_hs.cso", &hsBlob));
	HR(D3DReadFileToBlob(L"Terrain_ds.cso", &dsBlob));

	const D3D11_INPUT_ELEMENT_DESC desc[2] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	HR(device->CreateInputLayout(desc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_InputLayout));
	HR(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vsShader));
	HR(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_psShader));
	HR(device->CreateHullShader(hsBlob->GetBufferPointer(), hsBlob->GetBufferSize(), nullptr, &m_hsShader));
	HR(device->CreateDomainShader(dsBlob->GetBufferPointer(), dsBlob->GetBufferSize(), nullptr, &m_dsShader));

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(device->CreateSamplerState(&samplerDesc, &m_SamplerState));

	CreateConstantBuffer(device, sizeof(HS_PerFrame), &m_HS_CB_PerFrame);
	CreateConstantBuffer(device, sizeof(DS_PerFrame), &m_DS_CB_PerFrame);

	return true;
}

void TerrainEffects::Apply()
{
	ID3D11DeviceContext* context = WinGame::GetInstance()->GetContext();
	UpdateConstantBuffer<HS_PerFrame>(context, m_HS_PerFrame, m_HS_CB_PerFrame);
	UpdateConstantBuffer<DS_PerFrame>(context, m_DS_PerFrame, m_DS_CB_PerFrame);

	context->VSSetShader(m_vsShader, nullptr, 0);
	context->VSSetShaderResources(0, 1, &m_HeightSRV);
	context->VSSetSamplers(0, 1, &m_SamplerState);

	context->HSSetShader(m_hsShader, nullptr, 0);
	context->HSSetConstantBuffers(0, 1, &m_HS_CB_PerFrame);

	context->DSSetShader(m_dsShader, nullptr, 0);
	context->DSSetConstantBuffers(0, 1, &m_DS_CB_PerFrame);
	context->DSSetShaderResources(0, 1, &m_HeightSRV);
	context->DSSetSamplers(0, 1, &m_SamplerState);

	context->PSSetShader(m_psShader, nullptr, 0);
	context->PSSetShaderResources(0, 1, &m_BlendSRV);
	context->PSSetShaderResources(1, 1, &m_LayerSRV);
	context->PSSetSamplers(0, 1, &m_SamplerState);
}

void TerrainEffects::Clean()
{
	ID3D11DeviceContext* context = WinGame::GetInstance()->GetContext();
	context->VSSetShader(nullptr, 0, 0);
	context->PSSetShader(nullptr, 0, 0);
	context->DSSetShader(nullptr, 0, 0);
	context->HSSetShader(nullptr, 0, 0);
}

void TerrainEffects::CreateConstantBuffer(ID3D11Device * device, UINT size, ID3D11Buffer ** ppBuffer)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = size;
	HR(device->CreateBuffer(&desc, 0, ppBuffer));
}

template<typename T>
inline void TerrainEffects::UpdateConstantBuffer(ID3D11DeviceContext * context, T t, ID3D11Buffer * buffer)
{
	D3D11_MAPPED_SUBRESOURCE mappedSource;
	HR(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSource));
	*(T*)(mappedSource.pData) = t;
	context->Unmap(buffer, 0);
}