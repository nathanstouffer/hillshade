struct constants
{
    float4x4 view_proj;

    float4 bounds;
    float4 terrain_resolution;
    float4 albedo;

    float3 light_dir;
    float ambient_intensity;

    float3 eye;
    float exaggeration;

    float step_scalar;
    bool flag_3d;
};

struct VSInput
{
    float2 pos : ATTRIB0;
    float2 uv  : ATTRIB1;
};

struct PSInput 
{ 
    float4 pos       : SV_POSITION;
    float3 world_pos : POSITION;
    float2 uv        : TEX_COORD;
};

struct PSOutput
{ 
    float4 color : SV_TARGET; 
};