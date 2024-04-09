#include "stdafx.h"
#include "Chr_Battle_Vanila_State_Dead.h"
#include "Chr_Battle_Vanila.h"

CChr_Battle_Vanila_State_Dead::CChr_Battle_Vanila_State_Dead(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	m_pChr_Battle_Vanila = pChr_Battle_Vanila;
}

void CChr_Battle_Vanila_State_Dead::OnStateEnter()
{
}

void CChr_Battle_Vanila_State_Dead::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Vanila->Is_Animation_Finished()) {
		if (CChr_Battle_Vanila::DEAD_END == m_pChr_Battle_Vanila->Get_CurrentAnimationIndex())
			m_pChr_Battle_Vanila->Change_State(CChr_Battle_Vanila::IDLE);
		else {
			m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::DEAD_IDLE, true);
		}
	}

	if (m_pChr_Battle_Vanila->Get_CurrentAnimationIndex() == CChr_Battle_Vanila::DEAD_IDLE &&
		m_pChr_Battle_Vanila->Get_Hp() > 0) {
		m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::DEAD_END, false);
	}
}

void CChr_Battle_Vanila_State_Dead::OnStateExit()
{
}

CChr_Battle_Vanila_State_Dead* CChr_Battle_Vanila_State_Dead::Create(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	CChr_Battle_Vanila_State_Dead* pInstance = new CChr_Battle_Vanila_State_Dead(pChr_Battle_Vanila);

	return pInstance;
}

void CChr_Battle_Vanila_State_Dead::Free()
{
	__super::Free();
}
