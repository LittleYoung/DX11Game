cbuffer cbPerFrame
{
	float3 gEyePosW;
};

struct VertexOut
{
	float3 PosW : POSITION;
	float2 TexC : TEXCOORD;
};

struct PatchTess
{
	float EdgeTess[4]   : SV_TessFactor;
	float InsideTess[2] : SV_InsideTessFactor;
};

float CalcTessFactor(float3 posW)
{
	float dis = distance(posW, gEyePosW);
	float maxDis = 50;
	float minDis = 5;
	float maxTess = 6;
	float minTess = 0;
	float factor = saturate((maxDis - dis) / (maxDis - minDis));
	return pow(2, lerp(minTess, maxTess, factor));
}

PatchTess ConstantHS(InputPatch<VertexOut, 4> patch, uint patchID : SV_PrimitiveID)
{
	PatchTess pt;
	float3 e0 = 0.5f*(patch[0].PosW + patch[2].PosW);
	float3 e1 = 0.5f*(patch[0].PosW + patch[1].PosW);
	float3 e2 = 0.5f*(patch[1].PosW + patch[3].PosW);
	float3 e3 = 0.5f*(patch[2].PosW + patch[3].PosW);
	float3  c = 0.25f*(patch[0].PosW + patch[1].PosW + patch[2].PosW + patch[3].PosW);

	pt.EdgeTess[0] = CalcTessFactor(e0);
	pt.EdgeTess[1] = CalcTessFactor(e1);
	pt.EdgeTess[2] = CalcTessFactor(e2);
	pt.EdgeTess[3] = CalcTessFactor(e3);

	pt.InsideTess[0] = CalcTessFactor(c);
	pt.InsideTess[1] = pt.InsideTess[0];

	return pt;
}

struct HullOut
{
	float3 PosW : POSITION;
	float2 TexC : TEXCOORD;
};

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]
HullOut HS(InputPatch<VertexOut, 4> p,
	uint i : SV_OutputControlPointID,
	uint patchId : SV_PrimitiveID)
{
	HullOut hout;

	// Pass through shader.
	hout.PosW = p[i].PosW;
	hout.TexC = p[i].TexC;

	return hout;
}