#pragma once
#include <d3d11.h>
#include "Common\MathHelper.h"
#include "Common\GameMatrix.h"

class RainEffects
{
public:
	RainEffects();
	~RainEffects();

	bool Init();
	void InitShaders();

	void SetEyePosW(XMFLOAT3 v) { m_GSPerFrame.gEyePosW = v; }
	void SetDeltaTime(float t) { m_GSPerFrame.gDeltaTime = t; }
	void SetGameTime(float t) { m_GSPerFrame.gGameTime = t; }
	void SetRandomSRV(ID3D11ShaderResourceView* srv) { m_RandomSRV = srv; }
	void SetTexSRV(ID3D11ShaderResourceView* srv) { m_TexSRV = srv; }
	void SetViewProj(CXMMATRIX m) { m_gViewProj.Set(m); }
	void SetSOBufferStride(UINT n) { m_SO_BufferStride = n; }

	ID3D11InputLayout* GetLayout() { return m_InputLayout; }

	void ApplyStream();
	void ApplyDraw();
	void Clean();
private:
	void CreateConstantBuffer(ID3D11Device* device, UINT size, ID3D11Buffer** ppBuffer);
	template<typename T>
	void UpdateConstantBuffer(ID3D11DeviceContext* context, T t, ID3D11Buffer* buffer);

	struct GSPerFrame
	{
		XMFLOAT3 gEyePosW;
		float gDeltaTime;
		float gGameTime;
		XMFLOAT3 pad;
	};

	ID3D11VertexShader* m_Shader_VS_Stream;
	ID3D11VertexShader* m_Shader_VS_Draw;
	ID3D11GeometryShader* m_Shader_GS_Stream;
	ID3D11GeometryShader* m_Shader_GS_Draw;
	ID3D11PixelShader* m_Shader_PS_Draw;

	UINT m_SO_BufferStride;

	ID3D11InputLayout* m_InputLayout;

	ID3D11ShaderResourceView* m_RandomSRV;
	ID3D11SamplerState* m_SamplerState;
	ID3D11ShaderResourceView* m_TexSRV;

	GSPerFrame m_GSPerFrame;
	GameMatrix m_gViewProj;

	ID3D11Buffer* m_CB_GSPerFrame;
	ID3D11Buffer* m_CB_GSPerFrame_Draw;
};

