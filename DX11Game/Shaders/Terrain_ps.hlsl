Texture2D gBlendMap;
Texture2DArray gLayerMaps;
SamplerState samAnisotropic;

struct DomainOut
{
	float3 Posw : POSITION;
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD0;
	float2 TileTex : TEXCOORD1;
};

float4 PS(DomainOut dout) : SV_TARGET
{
	float4 blends = gBlendMap.Sample(samAnisotropic, dout.TexC);
	float4 color = gLayerMaps.Sample(samAnisotropic, float3(dout.TileTex, 0));
	[unroll]
	for (int i = 1; i < 5; i++)
	{
		float4 color2 = gLayerMaps.Sample(samAnisotropic, float3(dout.TileTex, i));
		color = lerp(color, color2, blends[i - 1]);
	}

	return color;
}