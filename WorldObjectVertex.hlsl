#include "VertexHeader.hlsli"
#include "WorldObjectCBuffer.hlsli"
#include "PerspectiveTransform.hlsli"

PixelInput Vertex(VertexInput input)
{
	input.position.xyz += entityPos.xyz;

	PixelInput output;

	output.position = PerspectiveTransform(
		input.position,
		world,
		view,
		projection
	);
	output.tex = input.tex;

	return output;
}