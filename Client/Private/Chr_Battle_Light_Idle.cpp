#include "stdafx.h"
#include "Chr_Battle_Light_Idle.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_Idle::CChr_Battle_Light_Idle(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_Idle::OnStateEnter()
{
	// 체력이 부족한 경우
	if (m_isHurt) {
		m_eState = HURT;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_START, false);
	}
	else {
		m_eState = IDLE;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
	}

}

void CChr_Battle_Light_Idle::OnStateUpdate(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_I)) {
		m_isHurt = !m_isHurt;
	}

	switch (m_eState) {
	case IDLE:
		Idle();
		break;
	case HURT:
		Hurt();
		break;
	}

}

void CChr_Battle_Light_Idle::OnStateExit()
{
}

void CChr_Battle_Light_Idle::Idle()
{

	// 시간 지나면 이동 할 것

	// 애니메이션 변경
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		if (m_pChr_Battle_Light->Get_CurrentAnimationIndex() == CChr_Battle_Light::IDLE_HURT_END) {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
		}
	}
	// 체력이 낮은 경우
	if (m_isHurt) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_START, false);
		m_eState = HURT;
	}


}

void CChr_Battle_Light_Idle::Hurt()
{
	// 체력이 높으면 원상 복구됨
	if (!m_isHurt) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_END, true);
	}

	// 애니메이션 순차 재생
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		if (m_pChr_Battle_Light->Get_CurrentAnimationIndex() == CChr_Battle_Light::IDLE_HURT_START) {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT, true);
		}
	}

	
}


CChr_Battle_Light_Idle* CChr_Battle_Light_Idle::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Idle* pInstance = new CChr_Battle_Light_Idle(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_Idle::Free()
{
	__super::Free();
}
