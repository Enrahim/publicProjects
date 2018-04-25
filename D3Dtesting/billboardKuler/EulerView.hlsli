cbuffer viewer {
	float3 viewPos : packoffset(c0) ;
	float2 viewDir : packoffset(c1) ; // (up-down, left-right)
};

float3 viewTransform(float3 pos) {
	float3 res = pos - viewPos;
		float4 cs = float4(cos(viewDir), sin(viewDir)); // claimed to be once cycle 
		float4 cs2 = float4(cs.xy, -cs.zw);
		res = float3(dot(cs.xz, res.xy), dot(cs2.xz, res.yx), res.z);
	res = float3(res.x, dot(cs.yw, res.yz), dot(cs2.yw, res.zy));
	return res;
}