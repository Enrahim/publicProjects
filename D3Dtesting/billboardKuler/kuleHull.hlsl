// Input control point
struct VS_CONTROL_POINT_OUTPUT
{
	float4 centrum : SV_POSITION;
	float3 uDir : TANGENT0;
	float3 vDir : TANGENT1;
	float3 color : COLOR0;
};

// Output control point
struct HS_CONTROL_POINT_OUTPUT
{
	float4 centrum : SV_POSITION;
	float3 uDir : TANGENT0;
	float3 vDir : TANGENT1;
	float3 color : COLOR0;
};

// Output patch constant data.
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[4]			: SV_TessFactor; // e.g. would be [4] for a quad domain
	float InsideTessFactor[2]			: SV_InsideTessFactor; // e.g. would be Inside[2] for a quad domain
	// TODO: change/add other stuff
};

#define NUM_CONTROL_POINTS 1

// Patch Constant Function
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	// Insert code to compute Output here
	Output.EdgeTessFactor[0] = 
		Output.EdgeTessFactor[1] = 
		Output.EdgeTessFactor[2] =
		Output.EdgeTessFactor[3] =
		Output.InsideTessFactor[0] =
		Output.InsideTessFactor[1] = 1.0; // e.g. could calculate dynamic tessellation factors instead

	return Output;
}

[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(1)]
[patchconstantfunc("CalcHSPatchConstants")]
HS_CONTROL_POINT_OUTPUT main( 
	InputPatch<VS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HS_CONTROL_POINT_OUTPUT Output;

	// Insert code to compute Output here
	Output.centrum = ip[i].centrum;
	Output.uDir = ip[i].uDir;
	Output.vDir = ip[i].vDir;
	Output.color = ip[i].color;

	return Output;
}
