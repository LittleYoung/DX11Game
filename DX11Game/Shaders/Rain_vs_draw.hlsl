#include "Rain.hlsl"

struct VertexOut
{
	float3 PosW : POSITION;
	uint Type : TYPE;
};

VertexOut VS(Particle vin)
{
	float3 gAccelW = { -1.0f, -9.8f, 0.0f };
	float t = vin.Age;

	VertexOut vout;
	vout.PosW = 0.5*gAccelW*t*t + vin.InitVelW*t + vin.InitPosW;
	vout.Type = vin.Type;
	return vout;
}