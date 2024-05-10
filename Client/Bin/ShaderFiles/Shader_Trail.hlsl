#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
texture2D g_Texture;

float4 g_vPosition[16];

vector g_vCamPosition;


float g_SizeY;

struct VS_IN
{
    float3 vPosition : POSITION;
};

struct VS_OUT
{
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;
    
    return Out;
}


struct GS_IN
{
};

struct GS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
};

/* 정점을 생성한다. */
[maxvertexcount(16* 6)]
void GS_MAIN(point GS_IN In[1], inout TriangleStream<GS_OUT> Vertices)
{
   //float3 vRight = normalize(g_WorldMatrix._11_12_13_14) * g_SizeY * 0.5f;
   //float3 vUp = normalize(g_WorldMatrix._21_22_23_24) * g_SizeY * 0.5f;
   //float3 vLook = normalize(g_WorldMatrix._31_32_33_34) * g_SizeY * 0.5f;
   // //float3 vUp = float3(0.f, 1.f, 0.f) * g_SizeY * 0.5f;
   // matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    float3 vLook = normalize((g_vCamPosition - g_WorldMatrix._41_42_43_44).xyz);
    float3 vRight = normalize(cross(float3(0.f, 1.f, 0.f), vLook.xyz));
    float3 vUp = normalize(cross(vLook, vRight)) * g_SizeY * 0.5f;

    matrix matVP = mul(g_ViewMatrix, g_ProjMatrix);
    
    for (int i = 0; i < 16-1; ++i)
    {
        GS_OUT Out[4];
        
        Out[0].vPosition = vector(g_vPosition[i].xyz + vUp, 1.f);
        Out[0].vTexcoord = float2((float) i / 16.f,0);
        Out[0].vPosition = mul(Out[0].vPosition, matVP);

        Out[1].vPosition = vector(g_vPosition[i + 1].xyz + vUp, 1.f);
        Out[1].vTexcoord = float2((float) (i + 1) / 16.f, 0);
        Out[1].vPosition = mul(Out[1].vPosition, matVP);

        Out[2].vPosition = vector(g_vPosition[i + 1].xyz - vUp, 1.f);
        Out[2].vTexcoord = float2((float) (i + 1) / 16.f, 0.5);
        Out[2].vPosition = mul(Out[2].vPosition, matVP);

        Out[3].vPosition = vector(g_vPosition[i].xyz - vUp, 1.f);
        Out[3].vTexcoord = float2((float) i / 16.f, 0.5);
        Out[3].vPosition = mul(Out[3].vPosition, matVP);

        Vertices.Append(Out[0]);
        Vertices.Append(Out[1]);
        Vertices.Append(Out[2]);
        Vertices.RestartStrip();

        Vertices.Append(Out[0]);
        Vertices.Append(Out[2]);
        Vertices.Append(Out[3]);
        Vertices.RestartStrip();
    }

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

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    //if (Out.vColor.a <= 0.1f)
    //    discard;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Default
    {
        SetRasterizerState(RS_CullNone);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = compile gs_5_0 GS_MAIN();
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }
}