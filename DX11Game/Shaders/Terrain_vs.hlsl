Texture2D gHeightMap;

SamplerState samAnisotropic;

struct VertexIn 
{
	float3 PosL : POSITION;
	float2 TexC : TEXCOORD;
};

struct VertexOut
{
	float3 PosW : POSITION;
	float2 TexC : TEXCOORD;
};

VertexOut VS(VertexIn vin) 
{
	VertexOut vout;
	vout.PosW = vin.PosL;
	float height = gHeightMap.SampleLevel(samAnisotropic, vin.TexC, 0).a;
	vout.PosW = float3(vin.PosL.x, height, vin.PosL.z);
	vout.TexC = vin.TexC;
	return vout;
}