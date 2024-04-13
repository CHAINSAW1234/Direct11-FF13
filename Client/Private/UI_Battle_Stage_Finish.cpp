#include "stdafx.h"
#include "UI_Battle_Stage_Finish.h"
#include "Player_Battle.h"

CUI_Battle_Stage_Finish::CUI_Battle_Stage_Finish(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Finish::OnStateEnter()
{
}

void CUI_Battle_Stage_Finish::OnStateUpdate(_float fTimeDelta)
{
}

void CUI_Battle_Stage_Finish::OnStateExit()
{
}

CUI_Battle_Stage_Finish* CUI_Battle_Stage_Finish::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Finish* pInstance = new CUI_Battle_Stage_Finish(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Finish::Free()
{
	__super::Free();
}
