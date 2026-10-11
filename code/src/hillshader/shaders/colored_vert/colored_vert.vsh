#include "shaders/colored_vert/structures.fxh"

cbuffer VSConstants
{
    constants g_vconstants;
};

void main(in VSInput vertex_input, out PSInput pixel_input) 
{
    pixel_input.pos = mul(g_vconstants.view_proj, float4(vertex_input.pos, 1.0));
    pixel_input.color = vertex_input.color;
}