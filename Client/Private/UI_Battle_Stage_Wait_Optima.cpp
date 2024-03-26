#include "stdafx.h"
#include "UI_Battle_Stage_Wait_Optima.h"
#include "Player_Battle.h"
#include "Chr_Battle_Light.h"

CUI_Battle_Stage_Wait_Optima::CUI_Battle_Stage_Wait_Optima(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Wait_Optima::OnStateEnter()
{
}

void CUI_Battle_Stage_Wait_Optima::OnStateUpdate(_float fTimeDelta)
{
	m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_SELECT);
}

void CUI_Battle_Stage_Wait_Optima::OnStateExit()
{
}

void CUI_Battle_Stage_Wait_Optima::Start()
{
}

CUI_Battle_Stage_Wait_Optima* CUI_Battle_Stage_Wait_Optima::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Wait_Optima* pInstance = new CUI_Battle_Stage_Wait_Optima(pPlayer_Battle);

	return S_OK;
}

void CUI_Battle_Stage_Wait_Optima::Free()
{
	__super::Free();
}
