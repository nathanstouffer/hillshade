struct constants
{
    float4x4 view_proj;
    float4 bounds;
    float4 terrain_resolution;
    float4 albedo;
    float3 light_dir;
    float ambient_intensity;
};

cbuffer VSConstants
{
    constants g_vconstants;
};

struct PSInput 
{ 
    float4 pos  : SV_POSITION; 
    float2 uv   : TEX_COORD;
};

void main(in uint vertex_id : SV_VertexID, out PSInput pixel_input) 
{
    float2 uvs[6];
    uvs[0] = float2(0.0, 0.0);
    uvs[1] = float2(0.0, 1.0);
    uvs[2] = float2(1.0, 1.0);
    uvs[3] = float2(1.0, 1.0);
    uvs[4] = float2(1.0, 0.0);
    uvs[5] = float2(0.0, 0.0);

    float2 pos = lerp(g_vconstants.bounds.xy, g_vconstants.bounds.zw, uvs[vertex_id]);
    pixel_input.pos = mul(g_vconstants.view_proj, float4(pos, 0.0, 1.0));
    pixel_input.uv  = uvs[vertex_id];
}