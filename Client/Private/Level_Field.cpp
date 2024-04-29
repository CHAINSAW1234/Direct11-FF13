#include "stdafx.h"
#include "Level_Field.h"
#include "Camera_Field.h"
#include "MapObject.h"

#include "Troup.h"
#include "Monster.h"
#include "Chr_Field.h"

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

	if (FAILED(Ready_Layer_Sky(TEXT("Layer_Sky"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Trigger(TEXT("Layer_Trigger"))))
		return E_FAIL;

	if (FAILED(Ready_Layer_Chr(g_strChrLayerTag)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Camera(g_strCameraLayerTag)))
		return E_FAIL;

	if (FAILED(Ready_Layer_Monster(g_strMonsterLayerTag)))
		return E_FAIL;

	m_pGameInstance->StopSound(CSound_Manager::BGM);

	static int iStart = true;
	if (iStart) {
		iStart = false;
		m_pGameInstance->PlaySoundLoop(TEXT("BGM_Field1.wav"), CSound_Manager::BGM_FIELD, SOUND_DEFAULT-0.2f);
	}
	else {
		m_pGameInstance->Pause(CSound_Manager::BGM_FIELD);
	}


	return S_OK;
}

void CLevel_Field::Tick(_float fTimeDelta)
{
    __super::Tick(fTimeDelta);

	for (auto& iter = m_Troups.begin(); iter != m_Troups.end(); )
	{
		if ((*iter)->Tick()) {
			Safe_Release(*iter);
			iter = m_Troups.erase(iter);
			Save_Troup();

			m_pGameInstance->Pause(CSound_Manager::BGM_FIELD);
			m_pGameInstance->PlayBGM(TEXT("BGM_Battle.wav"));
			return;
		}
		else
			++iter;
	}

}


HRESULT CLevel_Field::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Field 레벨입니다."));

	return S_OK;
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
	pObject->Tick_Navigation();
	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Camera(const wstring& strLayerTag)
{
	CCamera::CAMERA_DESC		CameraDesc{};

	CameraDesc.fFovy = XMConvertToRadians(60.0f);
	CameraDesc.fAspect = (_float)g_iWinSizeX / g_iWinSizeY;
	CameraDesc.fNear = 0.1f;
	CameraDesc.fFar = 1000.0f;
	CameraDesc.vEye = _float4(0.f, .5f, -1.f, 1.f);
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
	CChr_Field* pChr_field = dynamic_cast<CChr_Field*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, TEXT("Prototype_GameObject_Chr_Field")));
	if (pChr_field == nullptr)
		return E_FAIL;
	pChr_field->Get_Transform()->Set_State(CTransform::STATE_POSITION, _float4(65.f, 0.f, 61.f, 1.f));

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Monster(const wstring& strLayerTag)
{
	static _bool isStart = true;

	if (isStart) {
		isStart = false;
		if(FAILED(Ready_Layer_Monster_Start(g_strMonsterLayerTag)))
			return E_FAIL;

		return S_OK;
	}

	if (FAILED(Load_Troup()))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Trigger(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Trigger"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Sky(const wstring& strLayerTag)
{
	if (FAILED(m_pGameInstance->Add_Clone(g_Level, strLayerTag, TEXT("Prototype_GameObject_Sky"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Field::Ready_Layer_Monster_Start(const wstring& strLayerTag)
{
	wstring strPrototypeTag = L"";
	CTroup* pTroup = { nullptr };
	CMonster* pMonster = { nullptr };

#pragma region Troup1
	pTroup = CTroup::Create(m_pDevice, m_pContext);

	strPrototypeTag = TEXT("Prototype_GameObject_Leopard");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ 23.f,0.f,63.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ 27.f,0.f,67.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	m_Troups.push_back(pTroup);
#pragma endregion

#pragma region Troup2
	pTroup = CTroup::Create(m_pDevice, m_pContext);

	strPrototypeTag = TEXT("Prototype_GameObject_Warload");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -10.f,0.f,65.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	strPrototypeTag = TEXT("Prototype_GameObject_Solider_Gun");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -0.5f,0.f,70.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -0.5f,0.f,60.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	m_Troups.push_back(pTroup);
#pragma endregion

#pragma region Troup3
	pTroup = CTroup::Create(m_pDevice, m_pContext);

	strPrototypeTag = TEXT("Prototype_GameObject_Solider");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -100.f,0.f,50.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	strPrototypeTag = TEXT("Prototype_GameObject_Solider_Gun");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -108.f,0.f,57.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -108.f,0.f,62.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	m_Troups.push_back(pTroup);
#pragma endregion

#pragma region Troup4
	pTroup = CTroup::Create(m_pDevice, m_pContext);

	strPrototypeTag = TEXT("Prototype_GameObject_Leopard");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -65.f,0.f,95.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -65.f,0.f,100.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	strPrototypeTag = TEXT("Prototype_GameObject_Solider");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -65.f,0.f,93.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -70.f,0.f,95.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	strPrototypeTag = TEXT("Prototype_GameObject_Solider_Gun");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -75.f,0.f,95.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -61.f,0.f,97.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	m_Troups.push_back(pTroup);

#pragma endregion

#pragma region Troup5
	pTroup = CTroup::Create(m_pDevice, m_pContext);

	strPrototypeTag = TEXT("Prototype_GameObject_Warload");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ 58.f,0.f,192.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	strPrototypeTag = TEXT("Prototype_GameObject_Solider");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ 53.f,0.f,183.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ 55.f,0.f, 186.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	strPrototypeTag = TEXT("Prototype_GameObject_Solider_Gun");
	pMonster = dynamic_cast<CMonster*>(m_pGameInstance->Add_Clone_With_Object(g_Level, strLayerTag, strPrototypeTag));
	if (pMonster == nullptr)
		return E_FAIL;
	pMonster->Set_StartPosition(_float4{ -55.f,0.f,180.f,1.f });
	pTroup->Add_Monster(strPrototypeTag, pMonster);

	m_Troups.push_back(pTroup);
#pragma endregion

	return S_OK;
}

HRESULT CLevel_Field::Load_Troup()
{
	string filepath = "../Bin/DataFiles/Level_Field.dat";
	ifstream IFS{ filepath, ios::in | ios::binary };

	_float4 vPositon;
	IFS.read(reinterpret_cast<char*>(&vPositon), sizeof(_float4));
	((CTransform*)m_pGameInstance->Get_Component(LEVEL_FIELD, g_strChrLayerTag, g_strTransformTag, 0))->Set_State(CTransform::STATE_POSITION, vPositon);


	_int iNumTroup = 0;
	IFS.read(reinterpret_cast<char*>(&iNumTroup), sizeof(_int));

	for (int i = 0; i < iNumTroup; ++i) {
		CTroup* pTroup = CTroup::Create(m_pDevice, m_pContext, IFS);
		m_Troups.push_back(pTroup);
	}

	_float4x4 CamMatrix;
	IFS.read(reinterpret_cast<char*>(&CamMatrix), sizeof(_float4x4));
	((CTransform*)m_pGameInstance->Get_Component(g_Level, g_strCameraLayerTag, g_strTransformTag, 0))
		->Set_WorldMatrix(CamMatrix);


	IFS.close();

	return S_OK;
}

HRESULT CLevel_Field::Save_Troup()
{
	string filepath = "../Bin/DataFiles/Level_Field.dat";
	ofstream OFS{ filepath, ios::out | ios::binary };

  	_float4 vPlayerPosition = ((CTransform*)m_pGameInstance->Get_Component(LEVEL_FIELD, g_strChrLayerTag, g_strTransformTag, 0))->Get_State_Float4(CTransform::STATE_POSITION);
	OFS.write(reinterpret_cast<const char*>(&vPlayerPosition), sizeof(_float4));

	_int iNumTroup = (_int)m_Troups.size();
	OFS.write(reinterpret_cast<const char*>(&iNumTroup), sizeof(_int));
	for (auto& iter = m_Troups.begin(); iter != m_Troups.end(); ++iter)
		(*iter)->Save_Troup(OFS);


	_matrix CamWorldMatrix = m_pGameInstance->Get_Transform_Matrix_Inverse(CPipeLine::D3DTS_VIEW);


	OFS.write(reinterpret_cast<const char*>(&CamWorldMatrix), sizeof(_float4x4));


	OFS.close();
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

void CLevel_Field::Free()
{
	__super::Free();
	
	for (auto& pTroup : m_Troups)
		Safe_Release(pTroup);
	m_Troups.clear();



}
