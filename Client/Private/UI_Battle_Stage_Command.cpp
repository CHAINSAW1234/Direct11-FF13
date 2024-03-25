#include "stdafx.h"
#include "UI_Battle_Stage_Command.h"
#include "Player_Battle.h"
#include "Ability.h"
#include "UI_Pnal_Attack.h"

CUI_Battle_Stage_Command::CUI_Battle_Stage_Command(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Command::OnStateEnter()
{
	Update_Pnals();
}

void CUI_Battle_Stage_Command::OnStateUpdate(_float fTimeDelta)
{
	Update_Cursor();

	Update_KeyInput();

}

void CUI_Battle_Stage_Command::OnStateExit()
{
	for (auto& pPnal : m_Pnals)
		pPnal->Set_Render(false);
}

void CUI_Battle_Stage_Command::Start()
{
	Add_Pnals();
}

HRESULT CUI_Battle_Stage_Command::Add_Pnals()
{
	CUI_Pnal* pPnal = { nullptr };

	CUI_Pnal::UI_PNAL_DESC UI_Pnal_Desc = {};
	UI_Pnal_Desc.vStartPosition = { (_float)g_iWinSizeX * -0.5f + 15.f, -200.f + 25, 0.f };
	UI_Pnal_Desc.vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 200 + 15.f,-200.f + 25, 0.f };
	UI_Pnal_Desc.vTargetPosition.x += 128 * 1.3f + 5;
	UI_Pnal_Desc.vStartPosition.x += 128 * 1.3f + 5;
	UI_Pnal_Desc.strName = TEXT("");


	for (size_t i = 0; i < 4; ++i) {
		if (i % 2) {
			UI_Pnal_Desc.vTargetPosition.x += 128 * 1.3f + 5;
			UI_Pnal_Desc.vStartPosition.x += 128 * 1.3f + 5;
		}
		else {
			UI_Pnal_Desc.vTargetPosition.x -= 128 * 1.3f + 5;
			UI_Pnal_Desc.vStartPosition.x -= 128 * 1.3f + 5;
			UI_Pnal_Desc.vTargetPosition.x += 15;
			UI_Pnal_Desc.vStartPosition.x += 15;
			UI_Pnal_Desc.vStartPosition.y -= 25.f;
			UI_Pnal_Desc.vTargetPosition.y -= 25.f;
		}

		pPnal = dynamic_cast<CUI_Pnal*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Pnal"), TEXT("Prototype_GameObject_UI_Pnal"), &UI_Pnal_Desc));
		m_Pnals.push_back(pPnal);
	}


	for (auto& pPnal : m_Pnals) {
		Safe_AddRef(pPnal);
		pPnal->Set_Render(false);
	}

	return S_OK;
}

void CUI_Battle_Stage_Command::Update_Pnals()
{
	CRole* pRole = m_pPlayer_Battle->Get_Ability()->Get_CurrentRole();

	m_iPnalCount = (_int)pRole->Get_SkillSet_Count();
	for (size_t i = 0; i < pRole->Get_SkillSet_Count(); ++i) {
		CRole::SKILL eSkill = pRole->Get_Skill_Index(i);

		m_Pnals[i]->Set_Name(pRole->Get_SkillName(eSkill));
		m_Pnals[i]->Reset_Position();
		m_Pnals[i]->Set_Render(true);

	}

	for (int i = m_iPnalCount; i < m_Pnals.size(); ++i) {
		m_Pnals[i]->Set_Render(false);
	}

}

void CUI_Battle_Stage_Command::Update_Cursor()
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_DOWN)) {
		if (m_iCursor + 2 < m_iPnalCount) {
			m_iCursor += 2;
		}
	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_UP)) {
		if (m_iCursor - 2 >= 0) {
			m_iCursor -= 2;
		}
	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RIGHT)) {
		if (!(m_iCursor % 2) && m_iCursor < m_iPnalCount) {
			++m_iCursor;
		}
	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_LEFT)) {
		if ((m_iCursor % 2) && m_iPnalCount) {
			--m_iCursor;
		}
	}
	
	if(m_iCursor >= 0 && m_iCursor < m_iPnalCount )
		m_pPlayer_Battle->Set_CursorPosition(m_Pnals[m_iCursor]->Get_TargetPosition());

}

void CUI_Battle_Stage_Command::Update_KeyInput()
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RETURN)) {
		CRole* pRole = m_pPlayer_Battle->Get_Ability()->Get_CurrentRole();
		CRole::SKILL eSkill = pRole->Get_Skill_Index(m_iCursor);

		m_pPlayer_Battle->Check_Command_Insert(pRole->Get_Skill_Cost(eSkill));
		Create_Pnal_Attack();

		if (m_pPlayer_Battle->Check_Command_Full())
			m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_TARGET);
	}

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_BACKSPACE)) {
		if (m_pPlayer_Battle->Get_Command_empty())
			m_pPlayer_Battle->Back_Stage();
		else {
			m_pPlayer_Battle->Cancel_Command();

		}
	}

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_E)) {
		if (!m_pPlayer_Battle->Get_Command_empty())
			m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_TARGET);
	}

}

void CUI_Battle_Stage_Command::Create_Pnal_Attack()
{
	CRole* pRole = m_pPlayer_Battle->Get_Ability()->Get_CurrentRole();

	CRole::SKILL eSkill = pRole->Get_Skill_Index(m_iCursor);

	CUI_Pnal_Attack::UI_PNAL_ATTACK_DESC UI_Pnal_Attack_desc = {};

	UI_Pnal_Attack_desc.vStartPosition = { 0.f, -150.f, 0.f};
	UI_Pnal_Attack_desc.vTargetPosition = { 0.f, 0.f, 0.f };
	UI_Pnal_Attack_desc.eSkill = eSkill;
	UI_Pnal_Attack_desc.iSize = pRole->Get_Skill_Cost(eSkill);
	UI_Pnal_Attack_desc.strName = CRole::Get_SkillName(eSkill);

	CUI_Pnal_Attack* pPnal_Attack = dynamic_cast<CUI_Pnal_Attack*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Pnal_Attack"), &UI_Pnal_Attack_desc));

	if (nullptr == pPnal_Attack)
		return;

	m_pPlayer_Battle->Add_Command(pPnal_Attack);
}


CUI_Battle_Stage_Command* CUI_Battle_Stage_Command::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Command* pInstance = new CUI_Battle_Stage_Command(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Command::Free()
{
	__super::Free();

	for (auto& pPnal : m_Pnals)
		Safe_Release(pPnal);
	m_Pnals.clear();
}
