#include "stdafx.h"
#include "Chr_Battle_Vanila_State_Hit.h"
#include "Chr_Battle_Vanila.h"

CChr_Battle_Vanila_State_Hit::CChr_Battle_Vanila_State_Hit(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	m_pChr_Battle_Vanila = pChr_Battle_Vanila;
}

void CChr_Battle_Vanila_State_Hit::OnStateEnter()
{
	m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::HIT_STRAIGTH, false);
}

void CChr_Battle_Vanila_State_Hit::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Vanila->Is_Animation_Finished()) {
		if (m_pChr_Battle_Vanila->Get_Hp() == 0) {
			if (CChr_Battle_Vanila::HIT_FALL == m_pChr_Battle_Vanila->Get_CurrentAnimationIndex())
				m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::HIT_FALL_DEAD, false);
			else
				m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::DEAD_START, false);

			m_pChr_Battle_Vanila->Change_State(CChr_Battle_Vanila::DEAD);
		}
		else {
			m_pChr_Battle_Vanila->Change_State(CChr_Battle_Vanila::IDLE);
		}

	}
}

void CChr_Battle_Vanila_State_Hit::OnStateExit()
{
}

CChr_Battle_Vanila_State_Hit* CChr_Battle_Vanila_State_Hit::Create(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	CChr_Battle_Vanila_State_Hit* pInstance = new CChr_Battle_Vanila_State_Hit(pChr_Battle_Vanila);

	return pInstance;
}

void CChr_Battle_Vanila_State_Hit::Free()
{
	__super::Free();
}
