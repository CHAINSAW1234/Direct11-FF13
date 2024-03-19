#include "stdafx.h"
#include "Player_Battle.h"
#include "GameInstance.h"
#include "FSM.h"

#include "UI_Battle_Stage_Select.h"
#include "UI_Battle_Stage_Target.h"
#include "UI_Battle_Stage_Command.h"
#include "UI_Battle_Stage_Item.h"

#include "Chr_Battle.h"


CPlayer_Battle::CPlayer_Battle()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CPlayer_Battle::Change_Stage(UIStage eStage)
{
	m_eStage = eStage;
	m_pFSMCom->Change_State(eStage);
	//NotifyObserver();
}

HRESULT CPlayer_Battle::Initialize()
{
	if (FAILED(Add_Component_FSM()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer_Battle::Add_Component_FSM()
{
	CComponent* pComponent = m_pGameInstance->Clone_Component(LEVEL_STATIC, TEXT("Prototype_Component_FSM"));
	if (nullptr == pComponent)
		return E_FAIL;

	m_pFSMCom = dynamic_cast<CFSM*>(pComponent);
	if (nullptr == m_pFSMCom)
		return E_FAIL;

	m_pFSMCom->Add_State(STAGE_SELECT, CUI_Battle_Stage_Select::Create(this));

	Change_Stage(STAGE_SELECT);

	return S_OK;
}

void CPlayer_Battle::Start()
{
	m_pLeader = dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Chr"), 0));
	// ÀÎº¥Åä¸® ¾îÂ¼±¸ÀúÂ¼±¸
	// 
	// ¸ó½ºÅÍ ¾îÂ¼±¸ÀúÂ¼±¸

}

void CPlayer_Battle::Tick(_float fTimeDelta)
{
	m_pFSMCom->Update(fTimeDelta);

}

HRESULT CPlayer_Battle::Render()
{
	return S_OK;
}


CPlayer_Battle* CPlayer_Battle::Create()
{
	CPlayer_Battle* pInstance = new CPlayer_Battle();

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CPlayer_Battle"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer_Battle::Free()
{
	for (auto& pChr_Battle : m_Memebers)
		Safe_Release(pChr_Battle);
	m_Memebers.clear();

	Safe_Release(m_pFSMCom);
	Safe_Release(m_pGameInstance);

}
