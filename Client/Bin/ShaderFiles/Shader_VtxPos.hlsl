

/* �������� : ���̴� �ܺο� �ִ� �����͸� ���̴� ������ �޾ƿ´�. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
};


struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float  vCheck : PSIZE0;
};

/* ���� ���̴� */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vCheck = 0;
    if (In.vPosition.z == 50)
        Out.vCheck = 1;         // x��� ���� ��ġ
    else if (In.vPosition.x == 50)
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

    Out.vColor = float4(0, 0, 0, 1);
    if (In.vCheck == 1)
        Out.vColor = float4(1, 0, 0, 1);
    if (In.vCheck == 2)
        Out.vColor = float4(0, 1, 0, 1);
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}