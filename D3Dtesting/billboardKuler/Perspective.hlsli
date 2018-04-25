cbuffer perspective {
	float4 xyzn; //xscale, yscale, f/(f-n), near
};

float4 perspectiveTransform(float3 input) {
	input.z = input.z - xyzn.w;
	return float4(xyzn.xyz*input.xyz, input.z + xyzn.w);
}