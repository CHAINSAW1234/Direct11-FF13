#include "stdafx.h"
#include "Chr_Battle_Light_Hit.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_Hit::CChr_Battle_Light_Hit(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_Hit::OnStateEnter()
{
	if (m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y >= 1) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::HIT_AIR_START, false);
		m_eState = AIR;
	}
	else {
		// 충돌 방향에 따라 다른 애니메이션이 들어와야되는데 확인 방법이 아직 없다
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::HIT_STRAIGHT_RIGHT, false);
		m_eState = GROUND;
	}
}

void CChr_Battle_Light_Hit::OnStateUpdate(_float fTimeDelta)
{
	switch (m_eState) {
	case GROUND:
		Hit_Ground(fTimeDelta);
		break;
	case AIR:
		Hit_Air(fTimeDelta);
		break;
	}

}

void CChr_Battle_Light_Hit::OnStateExit()
{
}

void CChr_Battle_Light_Hit::Hit_Ground(_float fTimeDelta)
{
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		if (0) {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::DEAD_START, false);
			m_pChr_Battle_Light->Change_State(CChr_Battle_Light::DEAD);
		}
		else {
			m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
		}
	}
}

void CChr_Battle_Light_Hit::Hit_Air(_float fTimeDelta)
{
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		switch (m_pChr_Battle_Light->Get_CurrentAnimationIndex()) {
		case CChr_Battle_Light::HIT_AIR_START:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::HIT_AIR, true);
			break;
		case CChr_Battle_Light::HIT_FALL:
			if (0) {
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::HIT_FALL_DEAD, false);
			}
			else {
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::HIT_FALL_GETUP, false);
			}
			break;
		case CChr_Battle_Light::HIT_FALL_DEAD:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::DEAD_IDLE, true);
			m_pChr_Battle_Light->Change_State(CChr_Battle_Light::DEAD);
			break;
		case CChr_Battle_Light::HIT_FALL_GETUP:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, false);
			m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
			break;
		}

	}

	if (m_pChr_Battle_Light->Get_CurrentAnimationIndex() == CChr_Battle_Light::HIT_AIR) {
		m_pChr_Battle_Light->Get_Transform()->Go_Down(fTimeDelta);

		if (m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION).y < 1) {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::HIT_FALL, false);
		}
	}

}

CChr_Battle_Light_Hit* CChr_Battle_Light_Hit::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Hit* pInstance = new CChr_Battle_Light_Hit(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_Hit::Free()
{
	__super::Free();
}
