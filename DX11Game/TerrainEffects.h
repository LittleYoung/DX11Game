#pragma once
#include <d3d11.h>
#include "Common\GameMatrix.h"

class TerrainEffects 
{
public:
	TerrainEffects();
	~TerrainEffects();

	bool Init();

	void Apply();
	void Clean();

	void SetEyePosW(XMFLOAT3 p) { m_HS_PerFrame.gEyePosW = p; }
	void SetViewProj(CXMMATRIX m) { m_DS_PerFrame.gViewProj.Set(m); }
	void SetHeightSRV(ID3D11ShaderResourceView* srv) { m_HeightSRV = srv; }
	void SetLayerSRV(ID3D11ShaderResourceView* srv) { m_LayerSRV = srv; }
	void SetBlendSRV(ID3D11ShaderResourceView* srv) { m_BlendSRV = srv; }
	ID3D11InputLayout* GetLayout() { return m_InputLayout; }

private:
	struct HS_PerFrame
	{
		XMFLOAT3	gEyePosW;
		float		pad;
	};

	struct DS_PerFrame
	{
		GameMatrix	gViewProj;
	};

	void CreateConstantBuffer(ID3D11Device* device, UINT size, ID3D11Buffer** ppBuffer);

	template <typename T>
	void UpdateConstantBuffer(ID3D11DeviceContext* context, T t, ID3D11Buffer* buffer);


	ID3D11VertexShader*			m_vsShader;
	ID3D11PixelShader*			m_psShader;
	ID3D11HullShader*			m_hsShader;
	ID3D11DomainShader*			m_dsShader;

	ID3D11InputLayout*			m_InputLayout;
	ID3D11ShaderResourceView*	m_HeightSRV;
	ID3D11ShaderResourceView*	m_BlendSRV;
	ID3D11ShaderResourceView*	m_LayerSRV;

	ID3D11SamplerState*			m_SamplerState;

	HS_PerFrame					m_HS_PerFrame;
	DS_PerFrame					m_DS_PerFrame;

	ID3D11Buffer*				m_HS_CB_PerFrame;
	ID3D11Buffer*				m_DS_CB_PerFrame;
};