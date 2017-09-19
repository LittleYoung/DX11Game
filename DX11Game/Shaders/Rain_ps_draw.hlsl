Texture2D gTex;
SamplerState samLinear;

struct GeoOut
{
	float4 PosH  : SV_POSITION;
	float2 Tex   : TEXCOORD;
};

float4 PS(GeoOut gout) : SV_TARGET
{
	return gTex.Sample(samLinear, gout.Tex);
}