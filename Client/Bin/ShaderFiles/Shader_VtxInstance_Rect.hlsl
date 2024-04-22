#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;
texture2D g_DissolveTexture;

vector g_vCamPosition;

float g_DissolveTime;
float g_fColorMagnification;
float4 g_vColor;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;

    float4 vRight : TEXCOORD1;
    float4 vUp : TEXCOORD2;
    float4 vLook : TEXCOORD3;
    float4 vTranslation : TEXCOORD4;
    bool isLived : COLOR0;
};


struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    bool isLived : COLOR0;
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix LocalMatrix;

    LocalMatrix = float4x4(In.vRight, In.vUp, In.vLook, In.vTranslation);

    vector vPosition = mul(vector(In.vPosition, 1.f), LocalMatrix);

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vPosition, matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.isLived = In.isLived;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    bool isLived : COLOR0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = g_vColor * g_fColorMagnification;
    vColor.w = g_vColor.a;
    
    vector vDiffuseColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * vColor;

    if (vDiffuseColor.a <= 0.1f ||
		false == In.isLived)
        discard;
	
    Out.vColor = vDiffuseColor;
    
    return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = g_vColor * g_fColorMagnification;
    vColor.w = g_vColor.a;
    
    vector vDiffuseColor = g_Texture.Sample(LinearSampler, In.vTexcoord) * vColor;

    if (vDiffuseColor.a <= 0.1f ||
		false == In.isLived)
        discard;
	
    vector vDissolveColor = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    if (vDissolveColor.r < g_DissolveTime)
        discard;
    
    Out.vColor = vDiffuseColor;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Dissolve
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }
}