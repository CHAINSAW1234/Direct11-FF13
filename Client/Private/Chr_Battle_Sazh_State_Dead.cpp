#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Dead.h"
#include "Chr_Battle_Sazh.h"

CChr_Battle_Sazh_State_Dead::CChr_Battle_Sazh_State_Dead(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Dead::OnStateEnter()
{
	// 애니메이션을 지정하면 안됨
	// 히트중 사망시 그전 상태에 따라 다른 애니메이션이 전달됨
}

void CChr_Battle_Sazh_State_Dead::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Sazh->Is_Animation_Finished()) {
		if (CChr_Battle_Sazh::DEAD_END == m_pChr_Battle_Sazh->Get_CurrentAnimationIndex())
			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::IDLE);
		else {
			m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::DEAD_IDLE, true);
		}
	}

	if (m_pChr_Battle_Sazh->Get_Hp() > 0) {
		m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::DEAD_END, false);
	}

}

void CChr_Battle_Sazh_State_Dead::OnStateExit()
{
}

CChr_Battle_Sazh_State_Dead* CChr_Battle_Sazh_State_Dead::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	CChr_Battle_Sazh_State_Dead* pInstance = new CChr_Battle_Sazh_State_Dead(pChr_Battle_Sazh);

	return pInstance;
}

void CChr_Battle_Sazh_State_Dead::Free()
{
	__super::Free();
}
