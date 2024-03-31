#include "stdafx.h"
#include "Level_Field.h"
#include "Camera_Field.h"
#include "Camera_Free.h"
#include "MapObject.h"

#include "Troup.h"
#include "Monster.h"

#include "Level_Loading.h"

CLevel_Field::CLevel_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Field::Initialize()
{
    if (FAILED(__super::Initialize()))
        return E_FAIL;
    
	if (FAILED(Read_Map()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Chr(g_strChrLayerTag)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(g_strMonsterLayerTag)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(g_strCameraLayerTag)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Field::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	for (auto& pTroup : m_Troups)
		pTroup->Tick();
		//if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RETURN)) {
	//	m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BATTLE));
	//}

}


HRESULT CLevel_Field::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Field 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Field::Initialize_Parsed()
{
	return E_NOTIMPL;
}

HRESULT CLevel_Field::Read_Map()
{
	ifstream IFS{ strMapTag, ios::in | ios::binary };

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

HRESULT CLevel_Field::Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix)
{
	CMapObject::MAPOBJECT_DESC MapObjectDesc = {};
	MapObjectDesc.strModelTag = strModelTag;

	CMapObject* pObject = { nullptr };
	if (nullptr == (pObject = dynamic_cast<CMapObject*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_MapObject"), TEXT("Prototype_GameObject_MapObject"), &MapObjectDesc))))
		return E_FAIL;

	dynamic_cast<CTransform*>(pObject->Get_Component(g_strTransformTag))->Set_WorldMatrix(WorldMatrix);

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera_Field::CAMERA_FIELD_DESC		CameraDesc{};

	CameraDesc.fMouseSensor = 0.1f;
	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(0.f, 10.f, -7.f, 1.f);
	CameraDesc.vAt = _float4(0.f, 0.f, 0.f, 1.f);
	CameraDesc.fSpeedPerSec = 10.f;
	CameraDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Camera_Field"), &CameraDesc)))
		return E_FAIL;

	dynamic_cast<CCamera_Field*>(m_pGameInstance->Get_GameObject(g_Level, strLayerTag, 0))->Set_Target(
		m_pGameInstance->Get_GameObject(g_Level, g_strChrLayerTag, 0)
	);
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Chr(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Chr_Field"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Monster(const wstring& strLayerTag)
{
	CTroup* pTroup = CTroup::Create(m_pDevice, m_pContext);
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Leopard")));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4{ 1.f,0.f,1.f,1.f });
	pTroup->Add_Monster(TEXT("Prototype_GameObject_Leopard") , pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Leopard")));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4{2.f,0.f,1.f,1.f });
	pTroup->Add_Monster(TEXT("Prototype_GameObject_Leopard"), pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Leopard")));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4{ 3.f,0.f,1.f,1.f });
	pTroup->Add_Monster(TEXT("Prototype_GameObject_Leopard"), pMonster);

	m_Troups.push_back(pTroup);
	/*
	pTroup = CTroup::Create(m_pDevice, m_pContext);
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Leopard")));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4{ -1.f,0.f,1.f,1.f });
	pTroup->Add_Monster(TEXT("Prototype_GameObject_Leopard"), pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Leopard")));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4{ -2.f,0.f,1.f,1.f });
	pTroup->Add_Monster(TEXT("Prototype_GameObject_Leopard"), pMonster);

	m_Troups.push_back(pTroup);*/

	return S_OK;

}

CLevel_Field* CLevel_Field::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Field* pInstance = new CLevel_Field(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CLevel_Field* CLevel_Field::Create_Parsed(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Field* pInstance = new CLevel_Field(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Parsed()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Field"));

		Safe_Release(pInstance);
	}

	return pInstance;
}


void CLevel_Field::Free()
{
	__super::Free();
	
	for (auto& pTroup : m_Troups)
		Safe_Release(pTroup);
	m_Troups.clear();
}
