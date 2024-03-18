#include "stdafx.h"
#include "..\Public\Level_GamePlay.h"

#include "Terrain.h"
#include "Monster.h"
#include "Camera_Free.h"

CLevel_GamePlay::CLevel_GamePlay(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{

}

HRESULT CLevel_GamePlay::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Ready_Lights()))
		return E_FAIL;

	if(FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_LandObject()))
		return E_FAIL;

	//if (FAILED(Ready_Layer_Effect(TEXT("Layer_Effect"))))
	//	return E_FAIL;

	

	return S_OK;
}

void CLevel_GamePlay::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	CTerrain* pTerrain = dynamic_cast<CTerrain*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_BackGround"), 0));
	_float4 vPickingPos = { 0.f,0.f,0.f,0.f };
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_LBRACKET)) {
		if (pTerrain->Compute_Picking(&vPickingPos)) {
			CMonster* pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster")));
			((CTransform*)pMonster->Get_Component(g_strTransformTag))->Set_State(CTransform::STATE_POSITION, vPickingPos);
		}
	}


}

HRESULT CLevel_GamePlay::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("게임플레이레벨입니다."));

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Lights()
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

HRESULT CLevel_GamePlay::Ready_Layer_Camera(const wstring & strLayerTag)
{
	CCamera_Free::CAMERA_FREE_DESC		CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_LandMapObject(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_ForkLift"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_GamePlay::Ready_Layer_Player(const wstring & strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, strLayerTag, TEXT("Prototype_GameObject_Player_Study"))))
		return E_FAIL;

	return S_OK;
}

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

HRESULT CLevel_GamePlay::Ready_LandObject()
{
	///* 랜드오브젝트용 객체들에게 필요한 데이터를 구한다.*/
	/*CLandObject::LANDOBJECT_DESC		LandObjectDesc = {};
	LandObjectDesc.pTerrainTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), g_strTransformTag));
	LandObjectDesc.pTerrainVIBuffer = (CVIBuffer_Terrain*)(m_pGameInstance->Get_Component(LEVEL_GAMEPLAY, TEXT("Layer_BackGround"), TEXT("Com_VIBuffer")));	*/

	///* 구한정보들을 각 랜드오브젝트르 생성할 때 던진다. */
	//if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"), LandObjectDesc)))
	//	return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster"))))
		return E_FAIL;
	
	if (FAILED(Ready_Layer_LandMapObject(TEXT("Layer_LandMapObject"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Player(TEXT("Layer_Player"))))
		return E_FAIL;

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
