#include "EulerView.hlsli"
#include "Perspective.hlsli"

struct DS_OUTPUT
{
	float4 vPosition  : SV_POSITION;
	float3 color : COLOR0;
	// TODO: change/add other stuff
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

[domain("quad")]
DS_OUTPUT main(
	HS_CONSTANT_DATA_OUTPUT input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HS_CONTROL_POINT_OUTPUT, NUM_CONTROL_POINTS> patch)
{
	DS_OUTPUT Output;

	float3 tmp = patch[0].centrum.xyz + patch[0].uDir * domain.x + patch[0].vDir * domain.y;
		tmp = viewTransform(tmp);

	
	Output.vPosition = perspectiveTransform(tmp);
	Output.color = patch[0].color;

	return Output;
}
