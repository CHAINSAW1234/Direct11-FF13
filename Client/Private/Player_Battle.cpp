#include "stdafx.h"
#include "Player_Battle.h"
#include "GameInstance.h"
#include "FSM.h"

#include "UI_Battle_Stage_Select.h"
#include "UI_Battle_Stage_Target.h"
#include "UI_Battle_Stage_Target_Member.h"
#include "UI_Battle_Stage_Command.h"
#include "UI_Battle_Stage_Item.h"
#include "UI_Battle_Stage_Wait.h"

#include "UI_Pnal_Attack.h"
#include "UI_Pnal_Item.h"

#include "Chr_Battle_Light.h"
#include "Player_Study.h"
#include "Monster.h"
#include "Inventory.h"
#include "Ability.h"

CPlayer_Battle::CPlayer_Battle()
	: m_pGameInstance{ CGameInstance::Get_Instance() }
{
	Safe_AddRef(m_pGameInstance);
}

void CPlayer_Battle::Change_Stage(UISTAGE eStage)
{
	m_pFSMCom->Change_State(eStage);
	m_PrevStage.push(m_eStage);
	m_eStage = eStage;

	if (m_eStage == STAGE_SELECT) {				//Select로 돌아오면 스택 비우기
		while (!m_PrevStage.empty())
			m_PrevStage.pop();
	}


	NotifyObserver();
}

void CPlayer_Battle::Back_Stage()
{
	if (m_PrevStage.size() == 0)
		return;

	UISTAGE eStage = m_PrevStage.top();
	m_PrevStage.pop();

	m_eStage = eStage;
	m_pFSMCom->Change_State(eStage);
	NotifyObserver();

}

void CPlayer_Battle::Check_Command_Insert(_uint iCost)
{
	while (m_pLeader->Get_MaxATB() < m_iCommandCost + iCost)
		Cancel_Command();
}

_bool CPlayer_Battle::Check_Command_Full()
{
	if (m_pLeader->Get_MaxATB() == m_iCommandCost)
		return true;
	return false;
}

void CPlayer_Battle::Add_Command(CUI_Pnal_Attack* pPnal_Attack)
{
	pPnal_Attack->Start();

	_float3 vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 200, -150.f, 0.f };
	vTargetPosition.x += 64 * (pPnal_Attack->Get_Size()-1);

	vTargetPosition.x += 128.f  * (m_iCommandCost);

	pPnal_Attack->Set_TargetPosition(true, vTargetPosition);

	m_Commands.push_back(pPnal_Attack);
	Safe_AddRef(pPnal_Attack);

	Update_CommandCost();
}

void CPlayer_Battle::Cancel_Command()
{
	CUI_Pnal_Attack* pPnal_Attack = m_Commands.back();
	m_Commands.pop_back();

	pPnal_Attack->Set_TargetPosition(true, { 0.f, -150.f, 0.f});
	pPnal_Attack->Set_Dead(true);

	Safe_Release(pPnal_Attack);

	m_pLeader->Cancel_Command();
	Update_CommandCost();

}

void CPlayer_Battle::Use_Command()
{
	CUI_Pnal_Attack* pPnal_Attack = m_Commands.front();
	m_Commands.pop_front();

	pPnal_Attack->Set_TargetPosition(true, { (_float)g_iWinSizeX * -0.5f, -150.f, 0.f });
	pPnal_Attack->Set_Dead(true);

	Safe_Release(pPnal_Attack);
	Update_CommandCost();
	Update_CommandPosition();
}

_bool CPlayer_Battle::Check_Item()
{
	if (nullptr == m_pCommand_Item)
		return false;
	return true;
}

void CPlayer_Battle::Add_Item(CUI_Pnal_Item* pPnal_Item)
{
	pPnal_Item->Start();

	_float3 vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 200, -150.f, 0.f };
	vTargetPosition.x += 64;

	pPnal_Item->Set_TargetPosition(true, vTargetPosition);


	m_pCommand_Item = pPnal_Item;
	Safe_AddRef(pPnal_Item);

}

void CPlayer_Battle::Cancel_Item()
{
	if (nullptr == m_pCommand_Item)
		return;

	m_pCommand_Item->Set_TargetPosition(true, { 0.f, -150.f, 0.f });
	m_pCommand_Item->Set_Dead(true);
	Safe_Release(m_pCommand_Item);

	m_pCommand_Item = nullptr;
}

void CPlayer_Battle::Use_Item()
{
	if (nullptr == m_pCommand_Item)
		return;

	m_pCommand_Item->Set_TargetPosition(true, { (_float)g_iWinSizeX * -0.5f, -150.f, 0.f });
	m_pCommand_Item->Set_Dead(true);

	Safe_Release(m_pCommand_Item);
	m_pCommand_Item = nullptr;
}

void CPlayer_Battle::Set_Leader_Command()
{
	deque<pair<CRole::SKILL, _int>>* Actions = new deque<pair<CRole::SKILL, _int>>();


	for (auto& pCommand : m_Commands) {
		Actions->push_back({ pCommand->Get_Skill(),pCommand->Get_Size()});
	}

	m_pLeader->Set_Command(Actions);

}

void CPlayer_Battle::Set_Leader_Item()
{
	if (nullptr == m_pCommand_Item)
		return;

	CInventory::ITEM eItem = m_pCommand_Item->Get_Item();
	m_pLeader->Set_Item(eItem);

}

void CPlayer_Battle::Check_Leader_Action()
{
	if (m_pLeader->Get_Command_Size() < m_Commands.size()) {
		Use_Command();
	}
	if (nullptr != m_pCommand_Item && m_pLeader->Get_Item() == CInventory::ITEM_END) {
		Use_Item();
	}

}

void CPlayer_Battle::Set_CursorPosition(_float3 vCursorPosition)
{
	m_vCursorPosition = vCursorPosition;
	m_vCursorPosition.x -= (128 * 0.65f + 12);
	m_vCursorPosition.y -= 5.f;
	NotifyObserver();
}

void CPlayer_Battle::NotifyObserver()
{
	__super::NotifyObserver();
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
	m_pFSMCom->Add_State(STAGE_TARGET, CUI_Battle_Stage_Target::Create(this));
	m_pFSMCom->Add_State(STAGE_COMMAND, CUI_Battle_Stage_Command::Create(this));
	m_pFSMCom->Add_State(STAGE_ITEM, CUI_Battle_Stage_Item::Create(this));
	m_pFSMCom->Add_State(STAGE_TARGET_MEMBER, CUI_Battle_Stage_Target_Member::Create(this));
	m_pFSMCom->Add_State(STAGE_WAIT, CUI_Battle_Stage_Wait::Create(this));
	Change_Stage(STAGE_SELECT);

	return S_OK;
}

void CPlayer_Battle::Update_Monsters()
{
	// 몬스터가 사망시 삭제함
	for (auto& iter = m_Monsters.begin(); iter != m_Monsters.end(); )
	{
		if ((*iter)->Get_Dead())
		{
			Safe_Release(*iter);
			iter = m_Monsters.erase(iter);
		}
		else
			++iter;
	}
}

void CPlayer_Battle::Update_Command()
{
	_int iCost = 0;
	for (auto& pUI_Pnal_Attack : m_Commands) {
		iCost += pUI_Pnal_Attack->Get_Size();
		if (m_pLeader->Get_ATB() >= iCost) {
			pUI_Pnal_Attack->Set_Color({ 0.f,1.f,1.f,1.f });
		}
		else {
			pUI_Pnal_Attack->Set_Color({ 1.f,1.f,1.f,1.f });
		}
	}
}

void CPlayer_Battle::Update_CommandCost()
{
	m_iCommandCost = 0;
	for (auto& pPnal_Attack : m_Commands) {
		m_iCommandCost += pPnal_Attack->Get_Size();
	}

}

void CPlayer_Battle::Update_CommandPosition()
{
	_float3 vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 200, -150.f, 0.f };
	//vTargetPosition.x += 64 * (pPnal_Attack->Get_Size() - 1);
	//vTargetPosition.x += 128.f * (m_iCommandCost);

	_int iCost = 0;
	for (auto& pPnal_Attack : m_Commands) {

		_float3 vPos = vTargetPosition;
		vPos.x += 64 * (pPnal_Attack->Get_Size() - 1);
		vPos.x += 128.f * (iCost);
		iCost += pPnal_Attack->Get_Size();
		pPnal_Attack->Set_TargetPosition(false, vPos);
	}

}

void CPlayer_Battle::Start()
{
	m_pLeader = dynamic_cast<CChr_Battle_Light*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Chr"), 0));
	Safe_AddRef(m_pLeader);

	size_t iNum = m_pGameInstance->Get_LayerCnt(g_Level, TEXT("Layer_Chr"));
	for (size_t i = 1; i < iNum; ++i) {
		m_Memebers.push_back(dynamic_cast<CChr_Battle*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Chr"), (_uint)i)));
		Safe_AddRef(m_Memebers[i-1]);
	}

	// 인벤토리 어쩌구저쩌구
	// 
	// 몬스터 어쩌구저쩌구
	size_t iNumMonster = m_pGameInstance->Get_LayerCnt(g_Level, g_strMonsterLayerTag);
	
	for (size_t i = 0; i < iNumMonster; ++i) {
		CPlayer_Study* pMonster = dynamic_cast<CPlayer_Study*>(m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, (_uint)i));

		if (nullptr == pMonster)
			return;
		m_Monsters.push_back(pMonster);
		Safe_AddRef(pMonster);
	}

	m_pInventory = dynamic_cast<CInventory*>(m_pGameInstance->Get_GameObject(g_Level, TEXT("Layer_Inventory"), 0));
	Safe_AddRef(m_pInventory);

	m_pAbility = m_pLeader->Get_Ability();
	Safe_AddRef(m_pAbility);

	m_pFSMCom->Start();
	NotifyObserver();
}

void CPlayer_Battle::Tick(_float fTimeDelta)
{
	m_pFSMCom->Update(fTimeDelta);
	Update_Command();

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
	__super::Free();

	Safe_Release(m_pLeader);

	for (auto& pChr_Battle : m_Memebers)
		Safe_Release(pChr_Battle);
	m_Memebers.clear();

	for (auto& pMonster : m_Monsters)
		Safe_Release(pMonster);
	m_Monsters.clear();

	for (auto& pPnal_Attack : m_Commands)
		Safe_Release(pPnal_Attack);
	m_Commands.clear();

	Safe_Release(m_pCommand_Item);

	Safe_Release(m_pInventory);
	Safe_Release(m_pAbility);

	Safe_Release(m_pFSMCom);
	Safe_Release(m_pGameInstance);

}
