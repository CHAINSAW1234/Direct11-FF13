#include "stdafx.h"
#include "Boss_State_Idle.h"
#include "Boss.h"

CBoss_State_Idle::CBoss_State_Idle(CBoss* pBoss)
{
    m_pBoss = pBoss;
}

void CBoss_State_Idle::OnStateEnter()
{
	Change_State(IDLE);
	m_pBoss->Change_Animation(CBoss::IDLE, true);

	_float fDist = m_pBoss->Cal_Dist_Start();

	if (fDist <= 3.f) {
		Change_State(MOVE_BACK_JUMP);
		m_pBoss->Change_Animation(CBoss::JUMP_BACK, false);
	}
}

void CBoss_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	m_pBoss->Update_Attack_Time(fTimeDelta);
	switch (m_eState) {
	case IDLE:
		Idle(fTimeDelta);
		break;
	case MOVE_BACK_JUMP:
		Move_Back_Jump(fTimeDelta);
		break;
	case TURN:
		Turn(fTimeDelta);
		break;
	}
}

void CBoss_State_Idle::OnStateExit()
{
}

void CBoss_State_Idle::Idle(_float fTimeDelta)
{
	m_fDegree = m_pBoss->Cal_Degree_Target();

	if (m_fTimeDelta >= m_fStateTime) {
		if (abs(m_fDegree) > 30.f) {
			Change_State(TURN);
			if (m_fDegree >= 0)
				m_pBoss->Change_Animation(CBoss::TURN_LEFT, false);
			else
				m_pBoss->Change_Animation(CBoss::TURN_RIGHT, false);
		}
	}

	if ((abs(m_fDegree) < 30.f) && m_pBoss->Get_AttackTime() >= 5.f)
		m_pBoss->Update_Pattern();

	if (m_pBoss->Is_Animation_Finished())
		m_pBoss->Change_Animation(CBoss::IDLE, true);
}

void CBoss_State_Idle::Move_Back_Jump(_float fTimeDelta)
{
	if (5.f <= m_pBoss->Get_CurrentTrackPosition() &&
		m_pBoss->Get_CurrentTrackPosition() <= 20.f)
		m_pBoss->Get_Transform()->Go_Backward(fTimeDelta * 3);

	if (m_pBoss->Is_Animation_Finished()) {
		Change_State(IDLE);
		m_pBoss->Change_Animation(CBoss::IDLE, true);
	}
}

void CBoss_State_Idle::Turn(_float fTimeDelta)
{
	_float t1 = EaseOutCublic(m_fPrevTimeDelta / 1.5f);
	_float t2 = EaseOutCublic(m_fTimeDelta / 1.5f);

	m_pBoss->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2 - t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 1.5f) {
		Change_State(IDLE);
		m_pBoss->Change_Animation(CBoss::IDLE, true);
	}

	m_fPrevTimeDelta = m_fTimeDelta;
}

void CBoss_State_Idle::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
	m_fStateTime = Random_Float(2.f);
	m_fStateTime += 2.5f;
}

CBoss_State_Idle* CBoss_State_Idle::Create(CBoss* pBoss)
{
    CBoss_State_Idle* pInstance = new CBoss_State_Idle(pBoss);
    return pInstance;
}

void CBoss_State_Idle::Free()
{
    __super::Free();
}
