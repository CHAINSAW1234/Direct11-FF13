#include "Engine_Shader_Defines.hlsli"

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;
matrix g_CamViewMatrix, g_CamProjMatrix;
matrix g_LightViewMatrix, g_LightProjMatrix;
matrix g_ViewMatrixInv, g_ProjMatrixInv;

float g_Width;
float g_Height;

float3 g_SampleOffsets[32];

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

float gaussian(float x, float sigma)
{
    return (1.0 / (sqrt(2.0 * 3.14159) * sigma)) * exp(-(x * x) / (2.0 * sigma * sigma));
}

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

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    return Out;
}

struct PS_OUT_LIGHT
{
    float4 vShade : SV_TARGET0;
    float4 vSpecular : SV_TARGET1;
};

/* 빛 하나당 480000 수행되는 쉐이더. */

float3 randomNormal(float2 tex)
{
    float noiseX = (frac(sin(dot(tex, float2(15.8989f, 76.132f) * 1.0f)) * 46336.23745f));
    float noiseY = (frac(sin(dot(tex, float2(11.9899f, 62.223f) * 2.0f)) * 34748.34744f));
    float noiseZ = (frac(sin(dot(tex, float2(13.3238f, 63.122f) * 3.0f)) * 59998.47362f));
    return normalize(float3(noiseX, noiseY, noiseZ));
}

float CalcAmbientOcclusion(float2 Texcoord)
{
    
    float g_fRadius = 0.01;
    
    float3 vRay;
    float3 vReflect;
    float2 vRandomUV;
    float fOccNorm;
    
    int iColor = 0;
    
    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, Texcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    if (vNormal.a !=0)
        vNormal = float4(1.f, 1.f, 1.f, 1.f);
    
    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, Texcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;
    float fDepth = vDepthDesc.x * fViewZ;
    
    for (int i = 0; i < 32; ++i)
    {
        vRay = reflect(randomNormal(Texcoord), g_SampleOffsets[i]);
        vReflect = normalize(reflect(vRay, vNormal)) * g_fRadius;
        vReflect.x *= -1.f;
        vRandomUV = Texcoord + vReflect.xy;
        fOccNorm = g_DepthTexture.Sample(PointSampler, vRandomUV).x * g_DepthTexture.Sample(PointSampler, vRandomUV).y * 1000.f;
        
        if (fOccNorm <= fDepth)
            ++iColor;
        
        //float4 vReflect = reflect(normalize(vLightDir), vNormal);

        //float fSpecular = pow(max(dot(normalize(vLook) * -1.f, normalize(vReflect)), 0.f), 30.f);
        
    }
    
    return abs((iColor / 32.f));
    
}

PS_OUT_LIGHT PS_MAIN_DIRECTIONAL(PS_IN In)
{
    PS_OUT_LIGHT Out = (PS_OUT_LIGHT) 0;

    vector vNormalDesc = g_NormalTexture.Sample(PointSampler, In.vTexcoord);
    float4 vNormal = float4(vNormalDesc.xyz * 2.f - 1.f, 0.f);

    vector vDepthDesc = g_DepthTexture.Sample(PointSampler, In.vTexcoord);
    float fViewZ = vDepthDesc.y * 1000.0f;
	
    Out.vShade = CalcAmbientOcclusion(In.vTexcoord);
    //Out.vShade = g_vLightDiffuse * saturate(max(dot(normalize(g_vLightDir) * -1.f, vNormal), 0.f) + g_vLightAmbient * g_vMtrlAmbient);

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
    vector vSpecular =  g_SpecularTexture.Sample(LinearSampler, In.vTexcoord);

    Out.vColor = vDiffuse * vShade + vSpecular;

	/* 현재 픽셀의 월드상의 위치를 구한다. */

	/* ProjPos.w == View.Z */
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
    if (vPosition.w - 0.8f > (vLightDepthDesc.x * 2000.f))
        Out.vColor *= 0.2f;

    return Out;
}

struct PS_OUT_BLUR
{
    float4 vColor : SV_TARGET0;
};

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
    Out.vColor *= 1.5;
    return Out;
}

PS_OUT PS_BLURFINAL(PS_IN In)
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
    pass Debug
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

    pass Light_Directional
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

    pass Light_Point
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

    pass Final
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

    pass Blur   
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

    pass BlurFinal
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_NO_TEST_WRITE, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_BLURFINAL();
    }
}