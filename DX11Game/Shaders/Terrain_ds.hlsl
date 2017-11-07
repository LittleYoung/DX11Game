cbuffer PerFrame
{
	float4x4 gViewProj;
};

struct PatchTess
{
	float EdgeTess[4] : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

struct HullOut
{
	float3 PosW : POSITION;
	float2 TexC : TEXCOORD;
};

struct DomainOut
{
	float3 PosW : POSITION;
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD0;
	float2 TileTex : TEXCOORD1;
};

Texture2D gHeightMap;

SamplerState samAnisotropic;

[domain("quad")]
DomainOut DS(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
	DomainOut dout;

	dout.PosW = lerp(
		lerp(quad[0].PosW, quad[1].PosW, uv.x),
		lerp(quad[2].PosW, quad[3].PosW, uv.x),
		uv.y);

	dout.TexC = lerp(
		lerp(quad[0].TexC, quad[1].TexC, uv.x),
		lerp(quad[2].TexC, quad[3].TexC, uv.x),
		uv.y);

	dout.PosW.y = gHeightMap.SampleLevel(samAnisotropic, dout.TexC, 0).r;
	dout.PosH = mul(float4(dout.PosW, 1.0f), gViewProj);
	dout.TileTex = dout.TexC * 10;
	return dout;
}