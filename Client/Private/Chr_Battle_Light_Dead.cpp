#include "stdafx.h"
#include "Chr_Battle_Light_Dead.h"
#include "Chr_Battle_Light.h"
CChr_Battle_Light_Dead::CChr_Battle_Light_Dead(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_Dead::OnStateEnter()
{

}

void CChr_Battle_Light_Dead::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		switch (m_pChr_Battle_Light->Get_CurrentAnimationIndex()) {
		case CChr_Battle_Light::DEAD_START:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::DEAD_IDLE, true);
			break;
		case CChr_Battle_Light::DEAD_END:
			m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
			break;
		}
	}

	if (m_pChr_Battle_Light->Get_CurrentAnimationIndex() == CChr_Battle_Light::DEAD_IDLE) {
		if (0) {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::DEAD_END, false);
		}
	}

}

void CChr_Battle_Light_Dead::OnStateExit()
{
}

CChr_Battle_Light_Dead* CChr_Battle_Light_Dead::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Dead* pInstance = new CChr_Battle_Light_Dead(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_Dead::Free()
{
	__super::Free();
}
