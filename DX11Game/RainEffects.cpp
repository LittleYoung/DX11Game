#include "RainEffects.h"
#include <d3dcompiler.h>
#include "Common\GameUtil.h"
#include "DXApp.h"

RainEffects::RainEffects()
{
}


RainEffects::~RainEffects()
{
}

bool RainEffects::Init()
{
	InitShaders();
	return true;
}

void RainEffects::InitShaders()
{
	ID3D11Device* device = DXApp::GetInstance()->GetDevice();

	ID3DBlob* blob_vs_stream, *blob_vs_draw, *blob_gs_stream, *blob_gs_draw, *blob_ps_draw;

	HR(D3DReadFileToBlob(L"Rain_vs_stream.cso", &blob_vs_stream));
	HR(D3DReadFileToBlob(L"Rain_vs_draw.cso", &blob_vs_draw));
	HR(D3DReadFileToBlob(L"Rain_gs_stream.cso", &blob_gs_stream));
	HR(D3DReadFileToBlob(L"Rain_gs_draw.cso", &blob_gs_draw));
	HR(D3DReadFileToBlob(L"Rain_ps_draw.cso", &blob_ps_draw));

	HR(device->CreateVertexShader(blob_vs_stream->GetBufferPointer(), blob_vs_stream->GetBufferSize(), nullptr, &m_Shader_VS_Stream));
	HR(device->CreateVertexShader(blob_vs_draw->GetBufferPointer(), blob_vs_draw->GetBufferSize(), nullptr, &m_Shader_VS_Draw));
	HR(device->CreateGeometryShader(blob_gs_draw->GetBufferPointer(), blob_gs_draw->GetBufferSize(), nullptr, &m_Shader_GS_Draw));
	HR(device->CreatePixelShader(blob_ps_draw->GetBufferPointer(), blob_ps_draw->GetBufferSize(), nullptr, &m_Shader_PS_Draw));

	const D3D11_SO_DECLARATION_ENTRY soDesc[4] = {
		{ 0, "POSITION", 0, 0, 3, 0},
		{ 0, "VELOCITY", 0, 0, 3, 0 },
		{ 0, "AGE", 0, 0, 1, 0 },
		{ 0, "TYPE", 0, 0, 1, 0 }
	};

	HR(device->CreateGeometryShaderWithStreamOutput(blob_gs_stream->GetBufferPointer(), blob_gs_stream->GetBufferSize(), soDesc, 4, 
		&m_SO_BufferStride, 1, D3D11_SO_NO_RASTERIZED_STREAM, nullptr, &m_Shader_GS_Stream));


	const D3D11_INPUT_ELEMENT_DESC desc[4] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "VELOCITY", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "AGE", 0, DXGI_FORMAT_R32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TYPE", 0, DXGI_FORMAT_R32_UINT, 0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	HR(device->CreateInputLayout(desc, 4, blob_vs_stream->GetBufferPointer(), blob_vs_stream->GetBufferSize(), &m_InputLayout));

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;
	// allow use of all mip levels
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HR(device->CreateSamplerState(&samplerDesc, &m_SamplerState));

	CreateConstantBuffer(device, sizeof(GSPerFrame), &m_CB_GSPerFrame);
	CreateConstantBuffer(device, sizeof(GameMatrix), &m_CB_GSPerFrame_Draw);
}

void RainEffects::CreateConstantBuffer(ID3D11Device* device, UINT size, ID3D11Buffer** ppBuffer)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.ByteWidth = size;
	HR(device->CreateBuffer(&desc, 0, ppBuffer));
}

template<typename T>
void RainEffects::UpdateConstantBuffer(ID3D11DeviceContext* context, T t, ID3D11Buffer* buffer)
{
	D3D11_MAPPED_SUBRESOURCE mappedSource;
	HR(context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSource));
	*(T*)(mappedSource.pData) = t;
	context->Unmap(buffer, 0);
}


void RainEffects::ApplyStream()
{
	ID3D11DeviceContext* context = DXApp::GetInstance()->GetContext();
	
	UpdateConstantBuffer(context, m_GSPerFrame, m_CB_GSPerFrame);

	context->VSSetShader(m_Shader_VS_Stream, nullptr, 0);
	context->GSSetShader(m_Shader_GS_Stream, nullptr, 0);
	context->GSSetConstantBuffers(0, 1, &m_CB_GSPerFrame);
	context->GSSetShaderResources(0, 1, &m_RandomSRV);
	context->GSSetSamplers(0, 1, &m_SamplerState);
}

void RainEffects::ApplyDraw()
{
	ID3D11DeviceContext* context = DXApp::GetInstance()->GetContext();
	UpdateConstantBuffer(context, m_gViewProj, m_CB_GSPerFrame_Draw);

	context->VSSetShader(m_Shader_VS_Draw, nullptr, 0);
	context->GSSetShader(m_Shader_GS_Draw, nullptr, 0);
	context->GSSetConstantBuffers(0, 1, &m_CB_GSPerFrame_Draw);
	context->PSSetShader(m_Shader_PS_Draw, nullptr, 0);
	context->PSSetShaderResources(0, 1, &m_TexSRV);
	context->PSSetSamplers(0, 1, &m_SamplerState);
}

void RainEffects::Clean()
{
	ID3D11DeviceContext* context = DXApp::GetInstance()->GetContext();

	context->VSSetShader(nullptr, nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(nullptr, nullptr, 0);
}