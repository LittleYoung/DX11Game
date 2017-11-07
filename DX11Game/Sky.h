#pragma once
#include <d3d11.h>
#include "SkyEffects.h"

class Sky 
{
public:
	Sky();
	~Sky();

	bool						Init();
	void						InitState();
	void						InitMesh();
	void						Render();
private:
	ID3D11Buffer*				m_SkyVB;
	ID3D11Buffer*				m_SkyIB;

	UINT						m_SkyIndexCount;

	ID3D11DepthStencilState*	m_LessEqualDSS;
	ID3D11RasterizerState*		m_NoCullRS;
	ID3D11ShaderResourceView*	m_CubeMapSRV;

	SkyEffects					m_SkyEffects;
};