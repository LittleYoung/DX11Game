TextureCube gCubeMap;

SamplerState samTriLinearSam;

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float3 PosL : POSITION;
};

float4 PS(VertexOut vout) : SV_TARGET
{
	return gCubeMap.Sample(samTriLinearSam, vout.PosL);
}