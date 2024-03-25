

/* 전역변수 : 쉐이더 외부에 있는 데이터를 쉐이더 안으로 받아온다. */
matrix g_WorldMatrix, g_ViewMatrix, g_ProjMatrix;

texture2D g_Texture;
texture2D g_Texture_Border;
texture2D g_Texture_Mask;

float4 g_Color;

float g_SizeX;
float g_SizeY;

float g_Ratio;
float g_MaskMovement;

sampler LinearSampler = sampler_state
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};


struct VS_IN
{
    float3 vPosition : POSITION;
    float2 vTexcoord : TEXCOORD0;
};

struct VS_OUT
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoordMask : TEXCOORD1;
};

/* 정점 쉐이더 */
VS_OUT VS_MAIN(VS_IN In)
{
    VS_OUT Out = (VS_OUT) 0;

    matrix matWV, matWVP;

    matWV = mul(g_WorldMatrix, g_ViewMatrix);
    matWVP = mul(matWV, g_ProjMatrix);
    float2 vMask = { In.vTexcoord.x - g_MaskMovement , In.vTexcoord.y / g_SizeY + g_MaskMovement };
    
    Out.vPosition = mul(vector(In.vPosition, 1.f), matWVP);
    Out.vTexcoord = In.vTexcoord;
    Out.vTexcoordMask = vMask;
    return Out;
}


struct PS_IN
{
    float4 vPosition : SV_POSITION;
    float2 vTexcoord : TEXCOORD0;
    float2 vTexcoordMask : TEXCOORD1;
};

struct PS_OUT
{
    float4 vColor : SV_TARGET0;
};

PS_OUT PS_MAIN(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;

    Out.vColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
	
    if (0.3 > Out.vColor.a)
        discard;
    
    return Out;
}

PS_OUT PS_Border(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    Out.vColor = g_Texture_Border.Sample(LinearSampler, In.vTexcoord);  // border 텍스처가 적용됨
    
    if (0.3 > Out.vColor.a)
        discard;
    
    return Out;
}


PS_OUT PS_Inner(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vInnerTexture = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (0.3f >= vInnerTexture.a)
        discard;
    //float2 vMaskTexcoord = { In.vTexcoord.x, In.vTexcoord.y + g_MaskMovement };
    // 마스크 이동
    vector vMaskTexture = g_Texture_Mask.Sample(LinearSampler, In.vTexcoordMask);
    
    Out.vColor = vInnerTexture * g_Color - vMaskTexture * 0.3;

    return Out;
}

PS_OUT PS_Mask(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vInnerTexture = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (0.3f >= vInnerTexture.a)
        discard;
    //float2 vMaskTexcoord = { In.vTexcoord.x, In.vTexcoord.y + g_MaskMovement };
    // 마스크 이동
    vector vMaskTexture = g_Texture_Mask.Sample(LinearSampler, In.vTexcoordMask);
    
    // ration보다 작은 위치에서는 픽셀에 색, 마스크 적용
    if (In.vTexcoord.x < g_Ratio)
    {
        Out.vColor = vInnerTexture * g_Color - vMaskTexture * 0.3;
    }
    else
    {
        Out.vColor = vInnerTexture;
    }

    return Out;
}

PS_OUT PS_Disappear(PS_IN In)
{
    PS_OUT Out = (PS_OUT) 0;
    
    vector vTextureColor = g_Texture.Sample(LinearSampler, In.vTexcoord);
    if (0.3 > vTextureColor.a)
        discard;
    
    Out.vColor = vTextureColor * saturate((1 - g_Ratio * 2));
    
    return Out;
}

PS_OUT PS_Black_5(PS_IN In)     // 알파값 0.5를 먹여서 렌더링 한다
{
    PS_OUT Out = (PS_OUT) 0;
    Out.vColor = float4(0.f, 0.f, 0.f, 0.5f);

    return Out;
}

PS_OUT PS_Color_And_Ratio(PS_IN In)     // 특정 비율까지만 색을 렌더링 한다
{
    PS_OUT Out = (PS_OUT) 0;
    if (In.vTexcoord.x >= g_Ratio)
        discard;
    
    Out.vColor = g_Color;
    return Out;
}


PS_OUT PS_Color_And_Ratio_And_Mask(PS_IN In)     // 특정 비율까지만 색을 마스크를 먹여서 렌더링 한다
{
    PS_OUT Out = (PS_OUT) 0;
    
    if (In.vTexcoord.x >= g_Ratio)
        discard;
   
    Out.vColor = g_Color - saturate(cos(g_MaskMovement)) * 0.3;
    Out.vColor.w = 1.f;

    return Out;
}

technique11 DefaultTechnique
{
    pass Default                                            //0
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        GeometryShader = /*compile gs_5_0 GS_MAIN()*/NULL;
        HullShader = /*compile hs_5_0 HS_MAIN()*/NULL;
        DomainShader = /*compile ds_5_0 DS_MAIN()*/NULL;
        PixelShader = compile ps_5_0 PS_MAIN();
    }

    pass Inner                                              // 1
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Inner();
    }

    pass Mask                                               //2
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Mask();
    }

    pass Border                                             //3
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Border();
    }

    pass Disappear                                          //4
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Disappear();
    }


    pass Black_5 // 검은 색 사각형을 알파값 0.5로 렌더링  //5
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Black_5();
    }

    pass Color_And_Ratio // 전달한 색으로 특정 비율까지만 렌더링        //6
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Color_And_Ratio();
    }

    pass Color_And_Ratio_And_Mask // 전달한 색으로 특정 비율까지만 마스크를 포함하여 //7
    {
        VertexShader = compile vs_5_0 VS_MAIN();
        PixelShader = compile ps_5_0 PS_Color_And_Ratio_And_Mask();
    }


}