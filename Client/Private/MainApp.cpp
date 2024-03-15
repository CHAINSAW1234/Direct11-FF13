#include "stdafx.h"
#include "..\Public\MainApp.h"

#include "GameInstance.h"
#include "Level_Loading.h"
#include "FSM.h"

CMainApp::CMainApp()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);	

	//D3D11_SAMPLER_DESC

	/*m_pGraphic_Device->SetRenderState(D3DRS_TEXTUREFACTOR, 0x);*/
}


HRESULT CMainApp::Initialize()
{
	ENGINE_DESC		EngineDesc = {};

	EngineDesc.hWnd = g_hWnd;
	EngineDesc.isWindowed = true;
	EngineDesc.iWinSizeX = g_iWinSizeX;
	EngineDesc.iWinSizeY = g_iWinSizeY;

	/* 내 게임의 기초 초기화 과정을 거치자. */
	if (FAILED(m_pGameInstance->Initialize_Engine(g_hInst, LEVEL_END, EngineDesc, &m_pDevice, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_Prototype_GameObject()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Component_For_Static()))
		return E_FAIL;

	if (FAILED(Ready_Gara_Light()))
		return E_FAIL;

	LEVEL eLevel;

	//eLevel = LEVEL_GAMEPLAY;
	//eLevel = LEVEL_MAPTOOL;
	//eLevel = LEVEL_PARSING;
	eLevel = LEVEL_FIELD;

	if (FAILED(Open_Level(eLevel)))
		return E_FAIL;
	
	return S_OK;
}

void CMainApp::Tick(_float fTimeDelta)
{
	m_pGameInstance->Tick_Engine(fTimeDelta);
}

HRESULT CMainApp::Render()
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	m_pGameInstance->Draw();	

	return	S_OK;
}

HRESULT CMainApp::Open_Level(LEVEL eLevelID)
{
	if (LEVEL_LOADING == eLevelID)
		return E_FAIL;

	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID));

	return	S_OK;
}

HRESULT CMainApp::Ready_Prototype_GameObject()
{

	return S_OK;
}

HRESULT CMainApp::Ready_Prototype_Component_For_Static()
{

	/* For.Prototype_Component_VIBuffer_Rect */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	/* For.Prototype_Component_FSM */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_FSM"),
		CFSM::Create(m_pDevice, m_pContext))))
		return E_FAIL;

#pragma region Shader
	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxNorTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxNorTex.hlsl"), VTXNORTEX::Elements, VTXNORTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPosTex.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxModel.hlsl"), VTXMESH::Elements, VTXMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxModel */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimModel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxAnimModel.hlsl"), VTXANIMMESH::Elements, VTXANIMMESH::iNumElements))))
		return E_FAIL;

	/* For.Prototype_Component_Shader_VtxPosTex */
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPos"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Shader_VtxPos.hlsl"), VTXPOS::Elements, VTXPOS::iNumElements))))
		return E_FAIL;
#pragma endregion

	return	S_OK;
}

HRESULT CMainApp::Ready_Gara_Light()
{
	LIGHT_DESC			LightDesc{};

	LightDesc.eType = LIGHT_DESC::TYPE_DIRECTIONAL;
	LightDesc.vDirection = _float4(1.f, -1.f, 1.f, 0.f);

	LightDesc.vDiffuse = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vAmbient = _float4(1.f, 1.f, 1.f, 1.f);
	LightDesc.vSpecular = _float4(1.f, 1.f, 1.f, 1.f);

	if (FAILED(m_pGameInstance->Add_Light(LightDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMainApp::Ready_Gara()
{
	ID3D11Texture2D* pTexture2D = { nullptr };

	D3D11_TEXTURE2D_DESC	TextureDesc{};

	TextureDesc.Width = 256;
	TextureDesc.Height = 256;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DYNAMIC;
	TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	TextureDesc.MiscFlags = 0;

	_uint* pPixels = new _uint[256 * 256];
	ZeroMemory(pPixels, sizeof(_uint) * 256 * 256);

	for (_uint i = 0; i < 256; i++)
	{
		for (_uint j = 0; j < 256; j++)
		{
			_uint		iIndex = i * 256 + j;

			pPixels[iIndex] = /*0xffffffff*/D3DCOLOR_ARGB(255, 255, 255, 255);
		}
	}

	D3D11_SUBRESOURCE_DATA			SubResourceData{};

	SubResourceData.pSysMem = pPixels;
	SubResourceData.SysMemPitch = 256 * 4;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &SubResourceData, &pTexture2D)))
		return E_FAIL;

	D3D11_MAPPED_SUBRESOURCE		SubResource{};

	m_pContext->Map(pTexture2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource);

	for (_uint i = 0; i < 256; i++)
	{
		for (_uint j = 0; j < 256; j++)
		{
			_uint		iIndex = i * 256 + j;

			if (j < 128)
				pPixels[iIndex] = /*0xffffffff*/D3DCOLOR_ARGB(255, 255, 255, 255);
			else
				pPixels[iIndex] = /*0xffffffff*/D3DCOLOR_ARGB(255, 0, 0, 0);
		}
	}

	memcpy(SubResource.pData, pPixels, sizeof(_uint) * 256 * 256);

	m_pContext->Unmap(pTexture2D, 0);

	ID3D11ShaderResourceView* pSRV = { nullptr };

	if (FAILED(m_pDevice->CreateShaderResourceView(pTexture2D, nullptr, &pSRV)))
		return E_FAIL;

	if (FAILED(DirectX::SaveDDSTextureToFile(m_pContext, pTexture2D, TEXT("../Bin/Resources/Textures/Terrain/Mask.dds"))))
		return E_FAIL;

	Safe_Release(pSRV);
	Safe_Release(pTexture2D);

	Safe_Delete_Array(pPixels);

	return S_OK;
}

CMainApp * CMainApp::Create()
{
	CMainApp*		pInstance = new CMainApp();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CMainApp"));
		
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMainApp::Free()
{
	__super::Free();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);

	CGameInstance::Release_Engine();
}
