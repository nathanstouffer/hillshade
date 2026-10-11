struct constants
{
    float4x4 view_proj;
};

struct VSInput
{
    float3 pos   : ATTRIB0;
    float4 color : ATTRIB1;
};

struct PSInput
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR0;
};

struct PSOutput
{ 
    float4 color : SV_TARGET; 
};