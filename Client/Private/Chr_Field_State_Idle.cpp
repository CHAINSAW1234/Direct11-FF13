#include "stdafx.h"
#include "Chr_Field.h"
#include "Chr_Field_State_Idle.h"

CChr_Field_State_Idle::CChr_Field_State_Idle(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Idle::OnStateEnter()
{
	m_eState = WAIT;
	m_fTimeDelta = 0.f;
}

void CChr_Field_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	
	Update_Animation();
	if (m_fTimeDelta >= 5.f) {
		m_fTimeDelta = 0.f;
		switch (m_eState) {
		case IDLE:
			m_pChr_Field->Change_Animation(CChr_Field::IDLE_NOR_TO_LEFT_INARM, false);
			m_eState = LEFT_INARM;
			break;
		case LEFT_INARM: {
			int iNext = rand() % 2;
			if (iNext) {
				m_pChr_Field->Change_Animation(CChr_Field::IDLE_LEFT_INARM_TO_NOR, false);
				m_eState = IDLE;
			}
			else {
				m_pChr_Field->Change_Animation(CChr_Field::IDLE_LEFT_INARM_TO_RIGHT, false);
				m_eState = RIGHT_INARM;
			}
		}
			break;
		case RIGHT_INARM:
			m_pChr_Field->Change_Animation(CChr_Field::IDLE_RIGHT_INARM_TO_LEFT, false);
			m_eState = LEFT_INARM;
			break;
		}
	}

}

void CChr_Field_State_Idle::OnStateExit()
{
}

void CChr_Field_State_Idle::Update_Animation()
{
	if (!m_pChr_Field->Is_Animation_Finished())
		return;

	switch (m_pChr_Field->Get_CurrentAnimationIndex()) {
	case CChr_Field::IDLE_LEFT_INARM_TO_NOR:
		m_pChr_Field->Change_Animation(CChr_Field::IDLE_NOR, true);
		break;
	case CChr_Field::IDLE_LEFT_INARM_TO_RIGHT:
		m_pChr_Field->Change_Animation(CChr_Field::IDLE_RIGHT_INARM, true);
		break;
	case CChr_Field::IDLE_NOR_TO_LEFT_INARM:
		m_pChr_Field->Change_Animation(CChr_Field::IDLE_LEFT_INARM, true);
		break;
	case CChr_Field::IDLE_RIGHT_INARM_TO_LEFT:
		m_pChr_Field->Change_Animation(CChr_Field::IDLE_LEFT_INARM, true);
		break;
	default:
		m_pChr_Field->Change_Animation(CChr_Field::IDLE_NOR, true);
		m_eState = IDLE;
		break;
	}

	//m_fTimeDelta = 0.f;
}

CChr_Field_State_Idle* CChr_Field_State_Idle::Create(CChr_Field* pChr_Field)
{
	CChr_Field_State_Idle* pInstance = new CChr_Field_State_Idle(pChr_Field);
	//m_pPlayer = pGameObject;

	return pInstance;
}

void CChr_Field_State_Idle::Free()
{
	__super::Free();
}
