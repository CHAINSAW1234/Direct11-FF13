#include "stdafx.h"
#include "..\Public\Level_Loading.h"
#include "Loader.h"

#include "GameInstance.h"

#include "Level_Logo.h"
#include "Level_Field.h"
#include "Level_Battle.h"
#include "Level_Field_Boss.h"
#include "Level_Boss_Battle.h"
#include "Level_MapTool.h"
#include "Level_EffectTool.h"
#include "Level_Parsing.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CLevel{ pDevice, pContext }
{
}

HRESULT CLevel_Loading::Initialize(LEVEL eNextLevelID)
{
	if (FAILED(__super::Initialize()))
		return E_FAIL;

	if (eNextLevelID != LEVEL_LOGO) {
		if (FAILED(Ready_Layer_BackGround()))
			return E_FAIL;
	}

	m_eNextLevelID = eNextLevelID;

	/* 스레드를 생성하여 자원로드를 맡긴다. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;		

	return S_OK;
}



void CLevel_Loading::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	if (true == m_pLoader->isFinished())
	{
		CLevel*		pLevel = { nullptr };

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_FIELD:
			pLevel = CLevel_Field::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BATTLE:
			pLevel = CLevel_Battle::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_FIELD_BOSS:
			pLevel = CLevel_Field_Boss::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_BOSS_BATTLE:
			pLevel = CLevel_Boss_Battle::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_MAPTOOL:
			pLevel = CLevel_MapTool::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_EFFECTTOOL:
			pLevel = CLevel_EffectTool::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_PARSING:
			pLevel = CLevel_Parsing::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
			return;

		if (FAILED(m_pGameInstance->Open_Level(m_eNextLevelID, pLevel)))
			return;				
	}
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pLoader->Output();

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_BackGround()
{
	if (FAILED(m_pGameInstance->Add_Clone(LEVEL_LOADING, g_strBackGroundLayerTag, TEXT("Prototype_GameObject_LoadingBlur"))))
		return E_FAIL;

	return S_OK;
}

HRESULT CLevel_Loading::Ready_Layer_UI()
{

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Initialize(eNextLevelID)))
	{
		MSG_BOX(TEXT("Failed To Created : CLevel_Loading"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();

	Safe_Release(m_pLoader);
}
