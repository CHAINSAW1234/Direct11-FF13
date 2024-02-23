#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"





CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	//if (FAILED(Ready_Lights()))
	//	return E_FAIL;

	//if(FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
	//	return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	//if (FAILED(Ready_LandObject()))
	//	return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("�����÷��̷����Դϴ�."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
{
	


	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_LandObject()
{
	///* ���������Ʈ�� ��ü�鿡�� �ʿ��� �����͸� ���Ѵ�.*/
	//CLandObject::LANDOBJECT_DESC		LandObjectDesc = {};
	//LandObjectDesc.pTerrainTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag));
	//LandObjectDesc.pTerrainVIBuffer = (CVIBuffer_Terrain*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));	

	///* ������������ �� ���������Ʈ�� ������ �� ������. */
	//if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), LandObjectDesc)))
	//	return E_FAIL;

	///* ������������ �� ���������Ʈ�� ������ �� ������. */
	//if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"), LandObjectDesc)))
	//	return E_FAIL;

	return S_OK;
}
//
//HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag, CLandObject::LANDOBJECT_DESC& LandObjectDesc)
//{
//	/*if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player"), &LandObjectDesc)))
//		return E_FAIL;*/
//
//	return S_OK;
//}
//
//HRESULT CLevel_GamePlay::Ready_Layer_Monster(const wstring & strLayerTag, CLandObject::LANDOBJECT_DESC& LandObjectDesc)
//{
//	/*for (size_t i = 0; i < 20; i++)
//	{
//		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Monster"), &LandObjectDesc)))
//			return E_FAIL;
//	}
//	*/
//
//	return S_OK;
//}

HRESULT CLevel_GamePlay::Ready_Layer_Effect(const wstring & strLayerTag)
{
	/*for (size_t i = 0; i < 20; i++)
	{
		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Effect"))))
			return E_FAIL;
	}
*/

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_BackGround(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Terrain"))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
	//	return E_FAIL;

	return S_OK;
}

CLevel_GamePlay * CLevel_GamePlay::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_GamePlay*		pInstance = new CLevel_GamePlay(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_GamePlay"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_GamePlay::Free()
{
	__super::Free();
}
