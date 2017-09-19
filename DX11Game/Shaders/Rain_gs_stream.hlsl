#include "Rain.hlsl"

cbuffer PerFrame
{
	float3 gEyePosW;
	float gDeltaTime;
	float gGameTime;
};

Texture1D gRandomTex;
SamplerState samLinear;

float3 RandVec3(float offset)
{
	float u = gGameTime + gDeltaTime;
	float3 v = gRandomTex.SampleLevel(samLinear, u, 0).xyz;
	return v;
}

[maxvertexcount(6)]
void GS(point Particle gin[1], inout PointStream<Particle> ptStream)
{
	gin[0].Age += gDeltaTime;
	if (gin[0].Type == PT_EMITTER) {
		float age = gin[0].Age;
		float gap = 0.005f;
		if (age > gap) {
			gin[0].Age = 0;
		}
		ptStream.Append(gin[0]);
		if (age > 0.005f) {
			[unroll]
			for (int i = 0; i < 5; i++) {
				Particle p;
				float3 pos = 35.0f*RandVec3(i / 5.0f);
				pos.y = 20.0f;
				p.InitPosW = pos + gEyePosW;
				p.InitVelW = float3(0, 0, 0);
				p.Age = 0;
				p.Type = PT_FLARE;
				ptStream.Append(p);
			}
			gin[0].Age = 0;
		}
	} else {
		if (gin[0].Age < 3.0f) {
			ptStream.Append(gin[0]);
		}
	}
}