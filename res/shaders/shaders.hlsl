// res/shaders/shaders.hlsl
struct VertexInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VertexOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

// Constant buffer
cbuffer ConstantBuffer : register(b0)
{
    float4x4 model;
    float4x4 view;
    float4x4 proj;
};

VertexOutput VSMain(VertexInput input)
{
    VertexOutput output;
    
    // Transform: Model -> View -> Projection
    float4 worldPos = mul(float4(input.pos, 1.0f), model);
    float4 viewPos = mul(worldPos, view);
    output.pos = mul(viewPos, proj);
    
    output.color = input.color;
    
    return output;
}

float4 PSMain(VertexOutput input) : SV_TARGET
{
    return input.color;
}
