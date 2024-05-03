#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_CamViewMatrix, g_CamProjMatrix;
matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

float g_Width;
float g_Height;

texture2D g_Texture;
texture2D g_NormalTexture;
texture2D g_DiffuseTexture;
texture2D g_ShadeTexture;
texture2D g_DepthTexture;
texture2D g_SpecularTexture;
texture2D g_LightDepthTexture;
texture2D g_BlurTexture;
texture2D g_RandomNormalTexture;

float4 g_vLightDir;
float4 g_vLightPos;

float g_fLightRange;

float4 g_vLightDiffuse;
float4 g_vLightAmbient;
float4 g_vLightSpecular;

float4 g_vMtrlAmbient = float4(1.f, 1.f, 1.f, 1.f);
float4 g_vMtrlSpecular = float4(1.f, 1.f, 1.f, 1.f);

float4 g_vCamPosition;

struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float4 vWorldPos : TEXCOORD1;
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);

    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;

    return Out;
}

struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

struct PS_OUT_BLUR
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

float gaussian(float x, float sigma)
{
    return (1.0 / (sqrt(2.0 * 3.14159) * sigma)) * exp(-(x * x) / (2.0 * sigma * sigma));
}

float3 getPosition(in float2 uv)
{
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, uv);
    float fViewZ = vDepthDesc.y * 1000.0f;
    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = uv.x * 2.f - 1.f;
    vWorldPos.y = uv.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;
    
    vWorldPos *= fViewZ;
    
    matrix InverViewProj = mul(g_ProjMatrixInv, g_ViewMatrixInv);
    
    vWorldPos = mul(vWorldPos, InverViewProj);
    
    return mul(vWorldPos, g_ViewMatrix);
}

float doAmbientOcclusion(in float2 tcoord, in float2 uv, in float3 p, in float3 cnorm)
{
    float3 diff = getPosition(tcoord + uv) - p;
    const float3 v = normalize(diff);
    const float d = length(diff) * 0.5;   //g_scale
    return max(0.0, dot(cnorm, v) - 0.05) * (1.0 / (1.0 + d)) * 1.5f;
        // g_bias, g_intensity
}

float CalcAmbientOcclusion(float2 Texcoord)
{
    const float2 vec[4] = { float2(1, 0), float2(-1, 0), float2(0, 1), float2(0, -1) };
    float3 p = getPosition(Texcoord);
    float4 n = g_NormalTexture.Sample(PointSampler, Texcoord) * 2.f - 1.f;
    float2 rand = g_RandomNormalTexture.Sample(PointSampler, float2(1280, 720) * Texcoord / 900).xy * 2.f - 1.f;
    float ao = 0.0f;
    float rad = 0.5 / p.z; // 샘플링 반경 : 1 임시값
  
    //**SSAO Calculation**// 
    int iterations = 4;
    for (int j = 0; j < iterations; ++j)
    {
        float2 coord1 = reflect(vec[j], rand) * rad;
        float2 coord2 = float2(coord1.x * 0.707 - coord1.y * 0.707, coord1.x * 0.707 + coord1.y * 0.707);
    
        ao += doAmbientOcclusion(Texcoord, coord1 * 0.25, p, n);
        ao += doAmbientOcclusion(Texcoord, coord2 * 0.5, p, n);
        ao += doAmbientOcclusion(Texcoord, coord1 * 0.75, p, n);
        ao += doAmbientOcclusion(Texcoord, coord2, p, n);

    }
  

    ao /= (float) iterations * 4.0;

    return ao;
}

PS_OUT_LIGHT PS_MAIN_SSAO(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;
	
    //Out.vShade =  1 - CalcAmbientOcclusion(In.vTexcoord);
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (1 - CalcAmbientOcclusion(In.vTexcoord)) * g_vMtrlAmbient);

    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;
	
    //Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + (1 - CalcAmbientOcclusion(In.vTexcoord)) * g_vMtrlAmbient);
    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);

    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(g_vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;

    return Out;
}

PS_OUT_LIGHT PS_MAIN_POINT(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;

    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

    vector vLightDir = vWorldPos - g_vLightPos;
    float fDistance = length(vLightDir);

    float fAtt = saturate((g_fLightRange - fDistance) / g_fLightRange);

    Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);
    Out.vShade *= fAtt;
	
    float4 vLook = vWorldPos - g_vCamPosition;
    float4 vReflect = reflect(normalize(vLightDir), vNormal);

    float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);

    Out.vSpecular = (g_vLightSpecular * g_vMtrlSpecular) * fSpecular;
    Out.vSpecular *= fAtt;

    return Out;
}

/* 최종적ㅇ으로 480000 수행되는 쉐이더. */
PS_OUT PS_MAIN_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vDiffuse = g_DiffuseTexture.Sample(LinearSampler, In.vTexcoord);
    if (0.0f == vDiffuse.a)
        discard;
    vector vShade = g_ShadeTexture.Sample(LinearSampler, In.vTexcoord);
    vector vSpecular = g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular;

    return Out;
}

PS_OUT PS_SHADOW(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture.Sample(PointSampler, In.vTexcoord);
    
    if (Out.vColor.a ==0)
        discard;
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;

    float4 vWorldPos;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 / View.z */
    vWorldPos.x = In.vTexcoord.x * 2.f - 1.f;
    vWorldPos.y = In.vTexcoord.y * -2.f + 1.f;
    vWorldPos.z = vDepthDesc.x;
    vWorldPos.w = 1.f;

	/* 로컬위치 * 월드행렬 * 뷰행렬 * 투영행렬 */
    vWorldPos *= fViewZ;

	/* 로컬위치 * 월드행렬 * 뷰행렬 */
    vWorldPos = mul(vWorldPos, g_ProjMatrixInv);

	/* 로컬위치 * 월드행렬 */
    vWorldPos = mul(vWorldPos, g_ViewMatrixInv);

	/* 라이트 뷰, 투영을 곱한다. */
    vector vPosition = mul(vWorldPos, g_LightViewMatrix);
    vPosition = mul(vPosition, g_LightProjMatrix);
  
    float2 vTexcoord;

    vTexcoord.x = (vPosition.x / vPosition.w) * 0.5f + 0.5f;
    vTexcoord.y = (vPosition.y / vPosition.w) * -0.5f + 0.5f;
    
    vector vLightDepthDesc = g_LightDepthTexture.Sample(LinearSampler, vTexcoord);

	/* vPosition.w : 현재 내가 그릴려고 했던 픽셀의 광원기준의 깊이. */
	/* vLightDepthDesc.x * 2000.f : 현재 픽셀을 광원기준으로  그릴려고 했던 위치에 이미 그려져있떤 광원 기준의 깊이.  */
    if (vDepthDesc.x != 0 && 
        vPosition.w > (vLightDepthDesc.x * 2000.f))
        Out.vColor *= 0.2f;
  
    return Out;
}

const int iKernel_Size = 13;

PS_OUT_BLUR PS_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    float UVWidth = 1 / g_Width;
    float UVHeight = 1 / g_Height;
    float sigma = (iKernel_Size - 1)/6;
    float fTotalWeight = 0;
    for (int i = -iKernel_Size; i < iKernel_Size; ++i)
    {
        for (int j = -iKernel_Size; j < iKernel_Size; ++j)
        {
            float2 fTexcoord = { In.vTexcoord.x + UVWidth * i, In.vTexcoord.y + UVHeight * j };
            float fWeight = gaussian(length(fTexcoord), sigma);
            fTotalWeight += fWeight;
            Out.vColor += g_Texture.Sample(LinearSampler, fTexcoord) * fWeight;
        }
    }
    
    Out.vColor /= fTotalWeight;
    Out.vColor *= 1.5f;
    return Out;
}

PS_OUT PS_BLUR_FINAL(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vBrightColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    vector vBlurColor = g_BlurTexture.Sample(LinearSampler, In.vTexcoord);
    
    if (vBlurColor.a < 0.1)
        discard;
   
    if (vBrightColor.a < 0.1)
        Out.vColor = vBlurColor;
    else 
        Out.vColor = vBrightColor;
    
    return Out;
}

technique11 DefaultTechnique
{
    pass Debug  //0
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Light_Directional //1
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_DIRECTIONAL();
    }

    pass Light_Point    //2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Blend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_POINT();
    }

    pass Final //3
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_FINAL();
    }

    pass Blur      //4
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_BLUR();
    }

    pass Blur_Final  //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_BLUR_FINAL();
    }

    pass SSAO   //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN_SSAO();
    }

    pass Shadow //7
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_SHADOW();
    }
}