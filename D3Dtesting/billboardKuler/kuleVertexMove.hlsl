struct VertexIn {
	uint3 pos : POSITION0;
	uint3 vel : TEXCOORD0;
	float3 posNow : POSITION1;
	float3 velNow : TEXCOORD1;
};

struct VertexOut {
	float3 pos : POSITION;
	float3 vel : NORMAL;
	float4 mandPos : SV_POSITION;
};

cbuffer Time {
	float dt;
	float sinceUpdate;
};

#define vStiffnes  1.0
#define pStiffnes  vStiffnes * vStiffnes / 4 //Critical dampening

VertexOut main(VertexIn input)
{
	float3 shadePos = sinceUpdate * asfloat(input.vel) + asfloat(input.pos);
	float3 dv = pStiffnes * (shadePos - input.posNow) + vStiffnes * (asfloat(input.vel) - input.velNow);
	
	VertexOut tmp;
	tmp.vel = input.velNow + dt * dv;
	tmp.pos = input.posNow + dt * tmp.vel;
	tmp.mandPos = float4(tmp.pos, 1.0);
	
	return tmp;
}