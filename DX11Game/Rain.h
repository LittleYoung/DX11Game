#pragma once
#include <d3d11.h>
#include "RainEffects.h"

class Rain
{
public:
	Rain();
	~Rain();
	bool Init();

	void Render();

private:
	void InitTexSRV();
	void InitRandomTex();
	void InitVB();
	void InitState();

	ID3D11ShaderResourceView* m_RandomMapSRV;
	ID3D11ShaderResourceView* m_RainTexSRV;

	ID3D11Buffer* m_InitVB;
	ID3D11Buffer* m_DrawVB;
	ID3D11Buffer* m_StreamVB;

	ID3D11DepthStencilState* m_NoDepthDSS;

	RainEffects m_Effects;

	UINT m_MaxParticles;

	bool m_FirstRun;
};	

