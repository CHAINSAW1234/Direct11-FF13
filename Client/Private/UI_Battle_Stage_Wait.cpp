#include "stdafx.h"
#include "UI_Battle_Stage_Wait.h"
#include "Player_Battle.h"
#include "Chr_Battle_Light.h"

CUI_Battle_Stage_Wait::CUI_Battle_Stage_Wait(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Wait::OnStateEnter()
{
	m_pPlayer_Battle->Set_Leader_Command();
	m_pPlayer_Battle->Set_Leader_Item();
}

void CUI_Battle_Stage_Wait::OnStateUpdate(_float fTimeDelta)
 {
	Update_KeyInput();
	
	m_pPlayer_Battle->Check_Leader_Action();
	
	if (m_pPlayer_Battle->Get_Command_empty() && !m_pPlayer_Battle->Check_Item()) {				// 여기 플레이어가 IDLE이라는 조건 추가해야됨 
		m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_SELECT);
	}



	//m_pPlayer_Battle->Set_Leader_Action();

}

void CUI_Battle_Stage_Wait::OnStateExit()
{
}

void CUI_Battle_Stage_Wait::Start()
{
}

void CUI_Battle_Stage_Wait::Update_Cursor()
{
}

void CUI_Battle_Stage_Wait::Update_KeyInput()
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_BACKSPACE)) {
		if (m_pPlayer_Battle->Get_Command_size() > 1)
			m_pPlayer_Battle->Cancel_Command();
	}

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_E)) {
		while (m_pPlayer_Battle->Get_Command_Cost() > m_pPlayer_Battle->Get_Leader()->Get_ATB())
			m_pPlayer_Battle->Cancel_Command();
		//
	}

}

CUI_Battle_Stage_Wait* CUI_Battle_Stage_Wait::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Wait* pInstance = new CUI_Battle_Stage_Wait(pPlayer_Battle);
	return pInstance;
}

void CUI_Battle_Stage_Wait::Free()
{
	__super::Free();
}
