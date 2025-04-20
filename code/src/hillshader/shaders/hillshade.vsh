#include "shaders/structures.fxh"

Texture2D       g_terrain;
SamplerState    g_terrain_sampler;

cbuffer VSConstants
{
    constants g_vconstants;
};

void main(in VSInput vertex_input, out PSInput pixel_input) 
{
    float2 pos = lerp(g_vconstants.bounds.xy, g_vconstants.bounds.zw, vertex_input.pos);
    float elevation = (g_vconstants.flag_3d) ? g_terrain.Sample(g_terrain_sampler, vertex_input.uv).r : 0.0;
    float3 world_pos = float3(pos, elevation);
    pixel_input.pos = mul(g_vconstants.view_proj, float4(world_pos, 1.0));
    pixel_input.world_pos = world_pos;
    pixel_input.uv = vertex_input.uv;
}