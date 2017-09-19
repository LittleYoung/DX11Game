struct Particle
{
	float3 InitPosW : POSITION;
	float3 InitVelW : VELOCITY;
	float Age : AGE;
	uint Type : TYPE;
};

#define PT_EMITTER 0
#define PT_FLARE 1
