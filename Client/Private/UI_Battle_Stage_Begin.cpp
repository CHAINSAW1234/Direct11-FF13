#include "stdafx.h"
#include "UI_Battle_Stage_Begin.h"
#include "Player_Battle.h"

CUI_Battle_Stage_Begin::CUI_Battle_Stage_Begin(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Begin::OnStateEnter()
{
	m_fTimeDelta = 0.f;
}

void CUI_Battle_Stage_Begin::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	if (m_fTimeDelta >= 0.6f)
		m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_SELECT);
}

void CUI_Battle_Stage_Begin::OnStateExit()
{
}

CUI_Battle_Stage_Begin* CUI_Battle_Stage_Begin::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Begin* pInstance = new CUI_Battle_Stage_Begin(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Begin::Free()
{
	__super::Free();
}
