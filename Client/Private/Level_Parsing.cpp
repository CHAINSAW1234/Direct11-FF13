#include "stdafx.h"
#include "Level_Parsing.h"

#include "GameInstance.h"
#include "MapObject.h"

CLevel_Parsing::CLevel_Parsing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel { pDevice, pContext }
{
}

HRESULT CLevel_Parsing::Initialize()
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

#ifdef _DEBUG
	if (FAILED(Ready_Models()))
		return E_FAIL;
#endif
	return S_OK;
}

void CLevel_Parsing::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta); 

#ifdef _DEBUG
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_S))
		Parse_Models();

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_T))
		Test_Parsed_Models();
#endif

	// 테스트를 위해서 레벨이동하는 코드 추가하기?
}

HRESULT CLevel_Parsing::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("파싱 레벨입니다."));

	return S_OK;
}

#ifdef _DEBUG
HRESULT CLevel_Parsing::Ready_Models()
{
	SetWindowText(g_hWnd, TEXT("모델 로딩중."));

	string tag = ".fbx";
	string path = "";

	_matrix		TransformMatrix = XMMatrixIdentity();

	//TransformMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));
	//TransformMatrix.r[3].m128_f32[1] = -5.f;

	path = "../Bin/Resources/Models/MapObject/MapObject/Map_BossBattle_1.fbx";

	CModel* pModel = { nullptr };
	pModel = CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, path, TransformMatrix);
	if (nullptr == pModel)
		return E_FAIL;
	m_Models.push_back(pModel);

	return S_OK;
}


HRESULT CLevel_Parsing::Parse_Models()
{
	string path = "";

	path = "../Bin/Resources/Models/MapObject/MapObject/Map_BossBattle_1.bin";
	if (FAILED(m_Models[0]->Save_Model(path)))
		return E_FAIL;

	MessageBox(nullptr, TEXT("PARSING END"), L"Finish", MB_OK);
	return S_OK;
}

HRESULT CLevel_Parsing::Test_Parsed_Models()
{
	//// 1. MapObjects
	//for (size_t i = 0; i < 15; ++i) {
	//	string path = "../Bin/Resources/Models/MapObject/MapObject/map" + to_string(i) + ".bin";

	//	CModel* pModel = { nullptr };
	//	pModel = CModel::Create(m_pDevice, m_pContext, path);

	//	if (nullptr == pModel)
	//		return E_FAIL;
	//	m_Models.push_back(pModel);
	//}

	return S_OK;
}
#endif

CLevel_Parsing* CLevel_Parsing::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLevel_Parsing* pInstance = new CLevel_Parsing(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Parsing"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Parsing::Free()
{
	__super::Free();

	for (auto& pModel : m_Models) {
		Safe_Release(pModel);
	}
	m_Models.clear();
}
