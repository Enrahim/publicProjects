struct VertexIn {
	float3 pos : POSITION;
	float radius : TEXCOORD0;
	float3 color : COLOR0;
};

struct VertexOut {
	float4 centrum : SV_POSITION; 
	float3 uDir : TANGENT0;
	float3 vDir : TANGENT1;
	float3 color : COLOR0;
};

cbuffer viewer {
	float3 viewPos;
	float2 viewDir;
}

VertexOut main(VertexIn input)
{
	VertexOut res;
	float3 dir = input.pos - viewPos;
	float factor = sign(dir.x * dir.y) + 0.5;
	float4 tmp = float4(dir.x, dir.y, -dir.z, 0.0);
		float3 uDir = factor * tmp.zwx + tmp.wzy;
		float3 vDir = cross(dir, vDir);
		float l = length(dir);
	factor = input.radius * l / (l - input.radius); //simplification that makes it larger
	res.uDir = normalize(uDir) * factor ; 
	res.vDir = normalize(uDir) * factor ;
	res.centrum = float4(input.pos,1);
	res.color = input.color;
	return res;
}