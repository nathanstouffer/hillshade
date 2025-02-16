#include "shaders/structures.fxh"

cbuffer VSConstants
{
    constants g_vconstants;
};

void main(in uint vertex_id : SV_VertexID, out PSInput pixel_input) 
{
    float2 positions[6];
    positions[0] = float2(0.0, 0.0);
    positions[1] = float2(0.0, 1.0);
    positions[2] = float2(1.0, 1.0);
    positions[3] = float2(1.0, 1.0);
    positions[4] = float2(1.0, 0.0);
    positions[5] = float2(0.0, 0.0);

    float2 pos = lerp(g_vconstants.bounds.xy, g_vconstants.bounds.zw, positions[vertex_id]);
    pixel_input.pos = mul(g_vconstants.view_proj, float4(pos, 0.0, 1.0));
    pixel_input.uv  = float2(positions[vertex_id].x, 1.0 - positions[vertex_id].y);
}