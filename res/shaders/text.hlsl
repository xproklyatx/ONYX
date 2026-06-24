// text.hlsl - Font rendering shader with CBV

struct VSInput {
    float3 pos : POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VSOutput {
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

// Constant buffer (set via SetGraphicsRootConstantBufferView)
cbuffer TextConstants : register(b0) {
    float4x4 projection;
};

VSOutput VSMain(VSInput input) {
    VSOutput output;
    output.pos = mul(float4(input.pos, 1.0f), projection);
    output.uv = input.uv;
    output.color = input.color;
    return output;
}

Texture2D fontTexture : register(t0);
SamplerState fontSampler : register(s0);

float4 PSMain(VSOutput input) : SV_TARGET {
    float4 texColor = fontTexture.Sample(fontSampler, input.uv);
    return texColor * input.color;
}
