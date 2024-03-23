#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Hit.h"
#include "Chr_Battle_Sazh.h"

CChr_Battle_Sazh_State_Hit::CChr_Battle_Sazh_State_Hit(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Hit::OnStateEnter()
{
	// 방향에 따라 다른 이미지 나오게 처리해야함
	m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::HIT_STRAIGTH, false);
}

void CChr_Battle_Sazh_State_Hit::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Sazh->Is_Animation_Finished()) {
		if (m_pChr_Battle_Sazh->Get_Hp() == 0) {
			if (CChr_Battle_Sazh::HIT_FALL_LAND == m_pChr_Battle_Sazh->Get_CurrentAnimationIndex())
				m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::HIT_FALL_DEAD, false);
			else
				m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::DEAD_START, false);

			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::DEAD);
		}
		else {
			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::IDLE);
		}

	}
		
}

void CChr_Battle_Sazh_State_Hit::OnStateExit()
{
}

CChr_Battle_Sazh_State_Hit* CChr_Battle_Sazh_State_Hit::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	CChr_Battle_Sazh_State_Hit* pInstance = new CChr_Battle_Sazh_State_Hit(pChr_Battle_Sazh);

	return pInstance;
}

void CChr_Battle_Sazh_State_Hit::Free()
{
	__super::Free();
}
