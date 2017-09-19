#include "Rain.hlsl"

cbuffer PerFrame
{
	float4x4 gViewProj;
};

struct VertexOut
{
	float3 PosW : POSITION;
	uint Type : TYPE;
};

struct GeoOut
{
	float4 PosH  : SV_POSITION;
	float2 Tex   : TEXCOORD;
};

[maxvertexcount(2)]
void GS(point VertexOut vout[1], inout LineStream<GeoOut> lStream)
{
	float3 gAccelW = { -1.0f, -9.8f, 0.0f };

	if (vout[0].Type != PT_EMITTER)
	{
		float3 p0 = vout[0].PosW;
		float3 p1 = vout[0].PosW + 0.07f*gAccelW;

		GeoOut v0;
		v0.PosH = mul(float4(p0, 1.0f), gViewProj);
		v0.Tex = float2(0, 0);

		GeoOut v1;
		v1.PosH = mul(float4(p1, 1.0f), gViewProj);
		v1.Tex = float2(1, 1);

		lStream.Append(v0);
		lStream.Append(v1);
	}
}

