struct VertexIn {
	uint3 pos : POSITION;
	uint3 vel : TEXCOORD0;
};

struct VertexOut {
	float3 pos : POSITION;
	float3 vel : NORMAL;
	float4 mandPos : SV_Position;
};

VertexOut main( VertexIn input )
{
	VertexOut tmp;
	tmp.pos = asfloat(input.pos);
	tmp.vel = asfloat(input.vel);
	tmp.mandPos = float4(tmp.pos, 1);
	return tmp;
}