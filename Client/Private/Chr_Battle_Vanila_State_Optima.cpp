#include "stdafx.h"
#include "Chr_Battle_Vanila_State_Optima.h"
#include "Chr_Battle_Vanila.h"

CChr_Battle_Vanila_State_Optima::CChr_Battle_Vanila_State_Optima(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	m_pChr_Battle_Vanila = pChr_Battle_Vanila;
}

void CChr_Battle_Vanila_State_Optima::OnStateEnter()
{
	m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::OPTIMACHANGE, false);
}

void CChr_Battle_Vanila_State_Optima::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Vanila->Is_Animation_Finished()) {
		m_pChr_Battle_Vanila->Change_State(CChr_Battle_Vanila::IDLE);
	}
}

void CChr_Battle_Vanila_State_Optima::OnStateExit()
{
}

CChr_Battle_Vanila_State_Optima* CChr_Battle_Vanila_State_Optima::Create(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	CChr_Battle_Vanila_State_Optima* pInstance = new CChr_Battle_Vanila_State_Optima(pChr_Battle_Vanila);

	return pInstance;
}

void CChr_Battle_Vanila_State_Optima::Free()
{
	__super::Free();
}
