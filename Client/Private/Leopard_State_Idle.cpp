#include "stdafx.h"
#include "Leopard_State_Idle.h"
#include "Leopard.h"
#include "Chr_Battle.h"
CLeopard_State_Idle::CLeopard_State_Idle(CLeopard* pLeopard)
{
	m_pLeopard = pLeopard;
}

void CLeopard_State_Idle::OnStateEnter()
{
	m_fTimeDelta = 0.f;
	Change_State(IDLE);
	m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);
}

void CLeopard_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	m_pLeopard->Update_Attack_Time(fTimeDelta);
	switch (m_eState) {
	case IDLE:
		Idle(fTimeDelta);
		break;
	case MOVE:
		Move(fTimeDelta);
		break;
	case MOVE_BACK:
		Move_Back(fTimeDelta);
		break;
	case TURN:
		Turn(fTimeDelta);
		break;
	}

}

void CLeopard_State_Idle::OnStateExit()
{
}

void CLeopard_State_Idle::Idle(_float fTimeDelta)
{
	m_fDegree = m_pLeopard->Cal_Degree_Target();

	if (m_fTimeDelta >= m_fStateTime) {
		if (abs(m_fDegree) < 45.f) {
			_float fDist = m_pLeopard->Cal_Dist_Target();
			if (fDist <= 3.f) {
				Change_State(MOVE_BACK);
				m_pLeopard->Change_Animation(CLeopard::MOVE_BACK_START, false);
			}
			else if (fDist >= 10.f) {
				Change_State(MOVE);
				m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_START, false);
			}
			else {
				if (rand() % 2) {
					Change_State(MOVE);
					m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_START, false);
				}
				else {
					Change_State(MOVE_BACK);
					m_pLeopard->Change_Animation(CLeopard::MOVE_BACK_START, false);
				}
			}
		}
		else {
			Change_State(TURN);
			_int special = rand() % 4;
			if (0 == special) {			// 매우 낮은 확률로 나오게 처리함
				m_fDegree = m_pLeopard->Cal_Degree_Start();
				m_isMoveTurn = true;
			}

			if (m_fDegree >= 0)
				m_pLeopard->Change_Animation(CLeopard::TURN_LEFT, false);
			else 
				m_pLeopard->Change_Animation(CLeopard::TURN_RIGHT, false);
		}
	}

	if ((abs(m_fDegree) < 45.f) && m_pLeopard->Get_AttackTime() >= 5.f)
		m_pLeopard->Change_State(CLeopard::STATE_ATTACK);

	if (m_pLeopard->Is_Animation_Finished())
		m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);



}

void CLeopard_State_Idle::Move(_float fTimeDelta)
{
	m_pLeopard->Get_Transform()->Go_Straight(fTimeDelta, m_pLeopard->Get_Navigation());

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pLeopard->Get_CurrentTrackPosition()) == 13.f) {
			m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_STOP_LEFT, false);
			Change_State(IDLE);
		}
		else if (round(m_pLeopard->Get_CurrentTrackPosition()) == 27.f) {
			m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_STOP_RIGHT, false);
			Change_State(IDLE);
		}
	}

	if (2.f >= m_pLeopard->Cal_Dist_Target())
		m_fTimeDelta = m_fStateTime;

	if (m_pLeopard->Is_Animation_Finished()) {
		m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_IDLE, true);
	}
}

void CLeopard_State_Idle::Move_Back(_float fTimeDelta)
{
	m_pLeopard->Get_Transform()->Go_Backward(fTimeDelta, m_pLeopard->Get_Navigation());

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pLeopard->Get_CurrentTrackPosition()) == 13.f) {
			m_pLeopard->Change_Animation(CLeopard::MOVE_BACK_STOP_LEFT, false);
			Change_State(IDLE);
		}

		else if (round(m_pLeopard->Get_CurrentTrackPosition()) == 27.f) {
			m_pLeopard->Change_Animation(CLeopard::MOVE_BACK_STOP_RIGHT, false);
			Change_State(IDLE);
		}
	}

	if (10.f <= m_pLeopard->Cal_Dist_Target())
		m_fTimeDelta = m_fStateTime;

	if (m_pLeopard->Is_Animation_Finished()) {
		m_pLeopard->Change_Animation(CLeopard::MOVE_BACK_IDLE, true);
	}

}

void CLeopard_State_Idle::Turn(_float fTimeDelta)
{
	_float t1 = (_float)EaseOutCublic(m_fPrevTimeDelta / 2.f);
	_float t2 = (_float)EaseOutCublic(m_fTimeDelta / 2.f);

	m_pLeopard->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2-t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 2.f) {
		if (m_isMoveTurn) {
			m_isMoveTurn = false;
			Change_State(MOVE);
			m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_START, false);
		}
		else {
			Change_State(IDLE);
			m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);
		}
	}

	m_fPrevTimeDelta = m_fTimeDelta;
}

void CLeopard_State_Idle::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
	m_fStateTime = Random_Float(2.f);
	m_fStateTime += 2.5f;
}

CLeopard_State_Idle* CLeopard_State_Idle::Create(CLeopard* pLeopard)
{
	CLeopard_State_Idle* pInstance = new CLeopard_State_Idle(pLeopard);

	return pInstance;
}

void CLeopard_State_Idle::Free()
{
	__super::Free();
}
