#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

float2 g_TextureMovement;
float g_DissolveTime;
float4 g_vColor;
float g_fColorMagnification;

float4 g_vMaskStartValue;
float4 g_vMaskEndValue;

texture2D g_DiffuseTexture;
texture2D g_MaskTexture;
texture2D g_DissolveTexture;

struct VS_IN
{
    float3 vPosition : POSITION;
    float3 vNormal : NORMAL;
    float2 vTexcoord : TEXCOORD0;
    float3 vTangent : TANGENT;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    float2 vMask = { In.vTexcoord.x, In.vTexcoord.y };
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = float2(In.vTexcoord.x - g_TextureMovement.x, In.vTexcoord.y + g_TextureMovement.y);
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
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
    
    Out.vColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord) * vColor;
    if (Out.vColor.a < 0.1)
        discard;
    
    return Out;
}

PS_OUT PS_NOTEXTURE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_vColor;

    return Out;
}

PS_OUT PS_MASK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = g_vColor * g_fColorMagnification;
    vColor.w = g_vColor.a;
    
    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord) * vColor;
    if (vDiffuseColor.a < 0.1)
        discard;
    
    vector vMaskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (g_vMaskStartValue.x <= vMaskColor.r && vMaskColor.r <= g_vMaskEndValue.x &&
        g_vMaskStartValue.y <= vMaskColor.g && vMaskColor.g <= g_vMaskEndValue.y &&
        g_vMaskStartValue.z <= vMaskColor.b && vMaskColor.b <= g_vMaskEndValue.z &&
        g_vMaskStartValue.w <= vMaskColor.a && vMaskColor.a <= g_vMaskEndValue.w)
        discard;
    
    Out.vColor = vDiffuseColor;

    return Out;
}

PS_OUT PS_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float4 vColor = g_vColor * g_fColorMagnification;
    vColor.w = g_vColor.a;
    
    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord) * vColor;
    if (vDiffuseColor.a < 0.1)
        discard;
    
    vector vDissolveColor = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    
    if(vDissolveColor.r < g_DissolveTime)
        discard;
    
    Out.vColor = vDiffuseColor;

    return Out;
}

PS_OUT PS_MASK_DISSOLVE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    float4 vColor = g_vColor * g_fColorMagnification;
    vColor.w = g_vColor.a;
    
    vector vDiffuseColor = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord) * vColor;
    if (vDiffuseColor.a < 0.1)
        discard;
    
    vector vMaskColor = g_MaskTexture.Sample(LinearSampler, In.vTexcoord);
   
    if (g_vMaskStartValue.x <= vMaskColor.r && vMaskColor.r <= g_vMaskEndValue.x &&
        g_vMaskStartValue.y <= vMaskColor.g && vMaskColor.g <= g_vMaskEndValue.y &&
        g_vMaskStartValue.z <= vMaskColor.b && vMaskColor.b <= g_vMaskEndValue.z &&
        g_vMaskStartValue.w <= vMaskColor.a && vMaskColor.a <= g_vMaskEndValue.w)
        discard;
    
    vector vDissolveColor = g_DissolveTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vDissolveColor.r < g_DissolveTime)
        discard;
    
    
    Out.vColor = vDiffuseColor;

    return Out;
}

technique11 DefaultTechnique
{
    pass Default                                                    // 0 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass NoTexture                                                  // 1 
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_NOTEXTURE();
    }

    pass Texture_Mask                                               // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MASK();
    }

    pass Texture_Dissolve // 3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_DISSOLVE();
    }
    pass Texture_Mask_Dissolve // 4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MASK_DISSOLVE();
    }

}