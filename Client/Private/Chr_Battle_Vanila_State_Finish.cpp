#include "stdafx.h"
#include "Chr_Battle_Vanila_State_Finish.h"
#include "Chr_Battle_Vanila.h"

CChr_Battle_Vanila_State_Finish::CChr_Battle_Vanila_State_Finish(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	m_pChr_Battle_Vanila = pChr_Battle_Vanila;
}

void CChr_Battle_Vanila_State_Finish::OnStateEnter()
{
	m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::ANIM_FINISH, false);
	m_pChr_Battle_Vanila->Change_Animation_Weapon(CChr_Battle_Vanila::BATTLE_END, false);
}

void CChr_Battle_Vanila_State_Finish::OnStateUpdate(_float fTimeDelta)
{
}

void CChr_Battle_Vanila_State_Finish::OnStateExit()
{
}

CChr_Battle_Vanila_State_Finish* CChr_Battle_Vanila_State_Finish::Create(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	CChr_Battle_Vanila_State_Finish* pInstance = new CChr_Battle_Vanila_State_Finish(pChr_Battle_Vanila);

	return pInstance;
}

void CChr_Battle_Vanila_State_Finish::Free()
{
	__super::Free();
}
