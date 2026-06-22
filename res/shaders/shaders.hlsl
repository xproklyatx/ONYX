// res/shaders/shaders.hlsl
struct VertexInput
{
    float3 pos : POSITION;
    float4 color : COLOR;
	float2 uv : TEXCOORD;
};
Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);
struct VertexOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
	float2 uv : TEXCOORD;
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
    output.uv = input.uv;
    return output;
}

float4 PSMain(VertexOutput input) : SV_TARGET
{

    float4 texColor = g_texture.Sample(g_sampler, input.uv);
	return lerp(texColor, input.color, 0.2f);
}
