#include "shaders/colored_vert/structures.fxh"

cbuffer VSConstants
{
    constants g_vconstants;
};

void main(in VSInput vertex_input, out PSInput pixel_input) 
{
    pixel_input.pos = mul(float4(vertex_input.pos, 1.0), g_vconstants.view_proj);
    pixel_input.color = vertex_input.color;
}