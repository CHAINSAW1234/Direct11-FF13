#include "Engine_Shader_Defines.hlsli"

/* �������� : ���̴� �ܺο� �ִ� �����͸� ���̴� ������ �޾ƿ´�. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float  vCheck : PSIZE0; // �� üũ��
};

/* ���� ���̴� */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
  
    float4 WorldPosition = mul(vector(In.vPosition, 1.f), g_WorldMatrix);
    
    Out.vCheck = 0;
    if (WorldPosition.z == 0)
        Out.vCheck = 1;         // x��� ���� ��ġ
    else if (WorldPosition.x == 0)
        Out.vCheck = 2;         // z��� ���� ��ġ
    
    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float vCheck : PSIZE0;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = float4(0, 0, 0, 1);        // �⺻ �� : BLACK
    if (In.vCheck == 1)
        Out.vColor = float4(1, 0, 0, 1);    // x�� : RED
    if (In.vCheck == 2)
        Out.vColor = float4(0, 1, 0, 1);    // z�� : GREEN
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}