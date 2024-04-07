#include "stdafx.h"
#include "Level_Field_Boss.h"

#include "Camera_Field.h"
#include "MapObject.h"

#include "Troup.h"
#include "Monster.h"
#include "Chr_Field.h"
#include "Level_Loading.h"

CLevel_Field_Boss::CLevel_Field_Boss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Field_Boss::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (FAILED(Read_Map()))
		return E_FAIL;

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Chr(g_strChrLayerTag)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(g_strMonsterLayerTag)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(g_strCameraLayerTag)))
		return E_FAIL;

	return S_OK;
}

void CLevel_Field_Boss::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (!m_isBattleStart && m_pTroup->Tick()) {
		m_isBattleStart = true;
	}

	if (m_isBattleStart) {
		m_fTimeDelta += fTimeDelta;
		if (m_fTimeDelta >= 2.0f)
			m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, LEVEL_BOSS_BATTLE));
	}

}

HRESULT CLevel_Field_Boss::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Field 레벨입니다."));

	return S_OK;
}

HRESULT CLevel_Field_Boss::Read_Map()
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

HRESULT CLevel_Field_Boss::Create_MapObject(const wstring strModelTag, _float4x4 WorldMatrix)
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

HRESULT CLevel_Field_Boss::Ready_Layer_Camera(const wstring& strLayerTag)
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

HRESULT CLevel_Field_Boss::Ready_Layer_Chr(const wstring& strLayerTag)
{
	CChr_Field* pChr_field = dynamic_cast<CChr_Field*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Chr_Field")));
	if (pChr_field == nullptr)
		return E_FAIL;
	pChr_field->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4(0.f, 0.f, -20.f, 1.f));

	return S_OK;
}

HRESULT CLevel_Field_Boss::Ready_Layer_Monster(const wstring& strLayerTag)
{
	m_pTroup = CTroup::Create(m_pDevice, m_pContext);

	wstring strPrototypeTag = TEXT("Prototype_GameObject_Boss");
	CMonster* pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ 0.f,0.f,32.f,1.f });
	m_pTroup->Add_Monster(strPrototypeTag, pMonster);

	return S_OK;
}

HRESULT CLevel_Field_Boss::Ready_Layer_Sky(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

CLevel_Field_Boss* CLevel_Field_Boss::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Field_Boss* pInstance = new CLevel_Field_Boss(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Field_Boss"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Field_Boss::Free()
{
	__super::Free();

	Safe_Release(m_pTroup);
}
