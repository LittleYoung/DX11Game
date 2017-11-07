#pragma once
#include <d3d11.h>
#include <vector>
#include "TerrainEffects.h"

class Terrain 
{
public:
	Terrain();
	~Terrain();

	bool					Init();
	void					Render(bool ifWire);
	float					GetHeight(float x, float z) const;

private:
	void					LoadHeightMap();
	void					SmoothHeightMap();
	void					LoadHeightMapSRV();
	void					LoadTextures();
	void					InitPatchIABuffer();
	void					InitRState();

	UINT					m_HeightMapWidth;
	UINT					m_HeightMapHeight;
	float					m_CellSpace;
	float					m_HeightScale;
	std::vector<float>		m_HeightMap;

	UINT					m_NumPatchVertRows;
	UINT					m_NumPatchVertCols;

	ID3D11ShaderResourceView* m_LayerMapArraySRV;
	ID3D11ShaderResourceView* m_BlendMapSRV;
	ID3D11ShaderResourceView* m_HeightMapSRV;

	ID3D11Buffer*			m_vBuffer;
	ID3D11Buffer*			m_iBuffer;

	UINT					m_IndexCount;

	TerrainEffects			m_Effects;
	ID3D11RasterizerState*	m_WireRS;
	static const UINT		s_CellsPerPatch = 60;
};