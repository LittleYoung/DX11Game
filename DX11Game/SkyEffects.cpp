#include "SkyEffects.h"
#include <d3dcompiler.h>
#include "Common\GameUtil.h"
#include "Common\WinGame.h"

SkyEffects::SkyEffects()
{
}

SkyEffects::~SkyEffects()
{
}

bool SkyEffects::Init()
{
	ID3D11Device* device = WinGame::GetInstance()->GetDevice();

	ID3DBlob* vsBlob, *psBlob;
	D3DReadFileToBlob(L"sky_vs.cso", &vsBlob);
	D3DReadFileToBlob(L"sky_ps.cso", &psBlob);

	HR(device->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &m_vsShader));
	HR(device->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &m_psShader));

	const D3D11_INPUT_ELEMENT_DESC InputLayoutDesc[1] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HR(device->CreateInputLayout(InputLayoutDesc, 1, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &m_Layout));

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));

	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.ByteWidth = sizeof(GameMatrix);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	HR(device->CreateBuffer(&desc, nullptr, &m_buffer));

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	// allow use of all mip levels
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(device->CreateSamplerState(&samplerDesc, &m_SamplerState));

	return true;
}

void SkyEffects::Apply()
{
	ID3D11DeviceContext* context = WinGame::GetInstance()->GetContext();
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HR(context->Map(m_buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));
	GameMatrix* pData = reinterpret_cast<GameMatrix*>(mappedResource.pData);
	*pData = m_WorldViewProj;
	context->Unmap(m_buffer, 0);

	context->VSSetShader(m_vsShader, nullptr, 0);
	context->VSSetConstantBuffers(0, 1, &m_buffer);

	context->PSSetShader(m_psShader, nullptr, 0);
	context->PSSetSamplers(0, 1, &m_SamplerState);
	context->PSSetShaderResources(0, 1, &m_CubeMap);
}