#include "stdafx.h"
#include "Chr_Battle_Light_State_Optima.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_State_Optima::CChr_Battle_Light_State_Optima(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_State_Optima::OnStateEnter()
{
	m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::OPTIMACHANGE, false);
}

void CChr_Battle_Light_State_Optima::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Light->Is_Animation_Finished())
		m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
}

void CChr_Battle_Light_State_Optima::OnStateExit()
{
}

CChr_Battle_Light_State_Optima* CChr_Battle_Light_State_Optima::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_State_Optima* pInstance = new CChr_Battle_Light_State_Optima(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_State_Optima::Free()
{
	__super::Free();
}
