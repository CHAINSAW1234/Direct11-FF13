#include "Engine_Shader_Defines.hlsli"

/* �������� : ���̴� �ܺο� �ִ� �����͸� ���̴� ������ �޾ƿ´�. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D   g_Texture;
texture2D   g_MaskTexture;
float4      g_Color;

float       g_TimeDelta;

struct VS_IN
{
	float3		vPosition : POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

/* ���� ���̴� */
VS_OUT VS_MAIN(VS_IN In)
{
	VS_OUT		Out = (VS_OUT)0;

	matrix		matWV, matWVP;

	matWV = mul(g_WorldMatrix, g_ViewMatrix);
	matWVP = mul(matWV, g_ProjMatrix);

	Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
	Out.vTexcoord = In.vTexcoord;

	return Out;
}

struct PS_IN
{
	float4		vPosition : SV_POSITION;
	float2		vTexcoord : TEXCOORD0;
};

struct PS_OUT
{
	float4		vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
	PS_OUT			Out = (PS_OUT)0;

	Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a < 0.3)
        discard;
		
	return Out;
}

PS_OUT PS_COLOR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);

    if (Out.vColor.a < 0.3)
        discard;
		
    Out.vColor = Out.vColor * g_Color;
	
    return Out;
}

PS_OUT PS_COLOR2(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
		
    Out.vColor = Out.vColor * g_Color;
	
    return Out;
}


PS_OUT PS_MASK(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    
    if (Out.vColor.a < 0.3)
        discard;
		
    if (g_MaskTexture.Sample(LinearSampler, In.vTexcoord).a < 0.3)
        discard;
   
        return Out;
}

PS_OUT PS_RADIAL_BLUR(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    const float Strength = 0.125;
    const int Samples = 8;
    
    float fDist = distance(In.vTexcoord, float2(0.5f, 0.5f));
    float weight = 1.f - fDist;
    
    for (int i = 0; i < Samples; ++i) //operating at 2 samples for better performance
    {
        float2 offset = In.vTexcoord - (i * g_TimeDelta * 0.1 ) * (In.vTexcoord - float2(0.5f, 0.5f));
        Out.vColor += g_Texture.Sample(LinearSampler_Nonwrap, offset) * weight;
    }
    
    Out.vColor /= weight * Samples;
    Out.vColor += float4(1.f, 1.f, 1.f, 1.f) * g_TimeDelta;
   
    return Out;
}

PS_OUT PS_RADIAL_BLUR_REVERSE(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    const float Strength = 0.125;
    const int Samples = 8;
    
    float fDist = distance(In.vTexcoord, float2(0.5f, 0.5f));
    float weight = 1.f - fDist;
    
    for (int i = 0; i < Samples; ++i) //operating at 2 samples for better performance
    {
        float2 offset = In.vTexcoord - (i * (1 - g_TimeDelta * 2) * 0.1) * (In.vTexcoord - float2(0.5f, 0.5f));
        Out.vColor += g_Texture.Sample(LinearSampler_Nonwrap, offset) * weight;
    }
    
    Out.vColor /= weight * Samples;
    Out.vColor += float4(1.f, 1.f, 1.f, 1.f) * (1 - g_TimeDelta * 2);
   
    return Out;
}

technique11 DefaultTechnique
{
	pass Default
	{
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

		VertexShader = compile vs_5_0 VS_MAIN();
		GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
		HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
		DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
		PixelShader = compile ps_5_0 PS_MAIN();
	}

    pass Blend_Default
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


    pass Color  // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

    pass Blend_Color
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_AlphaBlend, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_COLOR();
    }

    pass Mask       //4
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

    pass Radial_Blur //5
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_RADIAL_BLUR();
    }

    pass Radial_Blur_Reverse //6
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_UI, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_RADIAL_BLUR_REVERSE();
    }

    pass Color2 // 2
    {
        SetRasterizerState(RS_Default);
        SetDepthStencilState(DSS_Default, 0);
        SetBlendState(BS_Default, float4(0.f, 0.f, 0.f, 0.f), 0xffffffff);

        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_COLOR2();
    }

}