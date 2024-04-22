#include "stdafx.h"
#include "Level_EffectTool.h"

#include "GameInstance.h"
#include "ImGUI_Manager.h"
#include "Camera_Free.h"
#include "EffectTool.h"
#include "MapObject.h"
#include "Solider.h"


CLevel_EffectTool::CLevel_EffectTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_EffectTool::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Read_Map()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(TEXT("Layer_Camera"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(TEXT("Layer_Monster"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_IMGUI(TEXT("Layer_IMGUI"))))
		return E_FAIL;

	return S_OK;
}

void CLevel_EffectTool::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

HRESULT CLevel_EffectTool::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Map_Tool 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_EffectTool::Read_Map()
{
	ifstream IFS{ "../Bin/Resources/Map/Level_Battle.dat", ios::in | ios::binary };

	size_t iModelNum = 0;
	IFS.read(reinterpret_cast<char*>(&iModelNum), sizeof(size_t));

	for (size_t i = 0; i < iModelNum; ++i) {
		size_t strModelTagLength;
		wstring strModelTag;

		IFS.read(reinterpret_cast<char*>(&strModelTagLength), sizeof(size_t));
		vector<_tchar> vecBuffer(strModelTagLength);

		IFS.read(reinterpret_cast<char*>(vecBuffer.data()), sizeof(_tchar) * strModelTagLength);
		strModelTag = wstring(vecBuffer.begin(), vecBuffer.end());

		_float4x4 WorldMatrix;
		IFS.read(reinterpret_cast<char*>(&WorldMatrix), sizeof(_float4x4));
		if (FAILED(Create_MapObject(strModelTag, WorldMatrix))) {
			return E_FAIL;
		}
	}
	IFS.close();
	return S_OK;
}

HRESULT CLevel_EffectTool::Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix)
{
	CMapObject::MAPOBJECT_DESC MapObjectDesc = {};
	MapObjectDesc.strModelTag = strModelTag;

	CMapObject* pObject = { nullptr };
	if (nullptr == (pObject = dynamic_cast<CMapObject*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_MapObject"), TEXT("Prototype_GameObject_MapObject"), &MapObjectDesc))))
		return E_FAIL;

	dynamic_cast<CTransform*>(pObject->Get_Component(g_strTransformTag))->Set_WorldMatrix(WorldMatrix);
	pObject->Tick_Navigation();
	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Camera(const wstring& strLayerTag)
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

	if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Camera_Free"), &CameraDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_BackGround(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Grid"))))
		return E_FAIL;
	return S_OK;
}

HRESULT CLevel_EffectTool::Ready_Layer_Monster(const wstring& strLayerTag)
{
	wstring strPrototypeTag = TEXT("Prototype_GameObject_Solider");
	CSolider* pMonster = dynamic_cast<CSolider*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ 0.f,0.f,0.f,1.f });
	pMonster->Change_State(CSolider::STATE_START);

	m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Boss"));


	return S_OK;

}

HRESULT CLevel_EffectTool::Ready_Layer_IMGUI(const wstring& strLayerTag)
{
	LEVEL eLevel = LEVEL_EFFECTTOOL;
	m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_EffectTool"), &eLevel);
	return S_OK;
}

CLevel_EffectTool* CLevel_EffectTool::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_EffectTool* pInstance = new CLevel_EffectTool(pDevice, pContext);
	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("너! CLevel_EffectTool 실패한고야...."));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_EffectTool::Free()
{
	__super::Free();
}
