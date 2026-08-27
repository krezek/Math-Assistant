#include "lighting.hlsl"

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

// Constant data that varies per pass.
cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gInvView;
    float4x4 gProj;
    
    float4x4 gInvProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    
    float3 gEyePosW;
    float cbPerObjectPad1;
};

struct VertexIn
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 ColorL : COLOR;
};

struct VertexOut
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float3 ColorW : COLOR;
};

VertexOut VS(VertexIn vin)
{
    VertexOut vout = (VertexOut) 0.0f;
	
    // Transform to world space.
    float4 posW = mul(float4(vin.PosL, 1.0f), gWorld);
    vout.PosW = posW.xyz;
    
    vout.NormalW = mul(vin.NormalL, (float3x3) gWorld);
    vin.NormalL = normalize(vin.NormalL);
    vout.NormalW = vin.NormalL;

    // Transform to homogeneous clip space.
    vout.PosH = mul(posW, gViewProj);
    
    vout.ColorW = vin.ColorL;
	
    return vout;
}

float4 PS(VertexOut pin) : SV_Target
{
    float4 ambient =
    {
        0.5f, 0.5f, 0.5f, 1.0f
    };
    Material mat =
    {
        { 1.0f, 1.0f, 1.0f, 1.0f },
        { 0.02f, 0.02f, 0.02f },
        0.8f
    };
    Light lgt =
    {
        { 0.19f, 0.19f, 0.19f },
        { 0.0f, -0.5f, 0.5f }
    };
    
    float3 toEyeW = normalize(gEyePosW - pin.PosW);
    lgt.Direction = -toEyeW;
    
    float4 directLight = ComputeLighting(lgt, mat, pin.NormalW, toEyeW);
    
    return float4(pin.ColorW, 1.0f) * ambient + directLight;
}
