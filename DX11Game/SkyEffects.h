#pragma once
#include <d3d11.h>
#include "Common\GameMatrix.h"

class SkyEffects 
{
public:
	SkyEffects();
	~SkyEffects();

	void SetWorldViewProj(CXMMATRIX M) { m_WorldViewProj.Set(M); }
	void SetCubeMap(ID3D11ShaderResourceView* tex) { m_CubeMap = tex; }
	ID3D11InputLayout* GetLayout() { return m_Layout; }

	bool Init();
	void Apply();
private:
	GameMatrix					m_WorldViewProj;

	ID3D11VertexShader*			m_vsShader;
	ID3D11PixelShader*			m_psShader;

	ID3D11Buffer*				m_buffer;
	ID3D11SamplerState*			m_SamplerState;
	ID3D11ShaderResourceView*	m_CubeMap;

	ID3D11InputLayout*			m_Layout;
};