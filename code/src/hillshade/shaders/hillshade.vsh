#include "shaders/structures.fxh"

cbuffer VSConstants
{
    constants g_vconstants;
};

void main(in VSInput vertex_input, out PSInput pixel_input) 
{
    float2 pos = lerp(g_vconstants.bounds.xy, g_vconstants.bounds.zw, vertex_input.pos);
    pixel_input.pos = mul(g_vconstants.view_proj, float4(pos, 0.0, 1.0));
    pixel_input.world_pos = float3(pos, 0.0);
    pixel_input.uv = vertex_input.uv;
}