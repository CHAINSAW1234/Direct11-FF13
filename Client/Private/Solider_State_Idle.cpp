#include "stdafx.h"
#include "Solider_State_Idle.h"
#include "Solider.h"
#include "Chr_Battle.h"

CSolider_State_Idle::CSolider_State_Idle(CSolider* pSolider)
{
    m_pSolider = pSolider;
}

void CSolider_State_Idle::OnStateEnter()
{
	Change_State(IDLE);
	m_pSolider->Change_Animation(CSolider::BATTLE_IDLE, true);
	if (m_pSolider->Get_IsJumpNext()) {
		m_pSolider->Set_IsJumpNext(false);
		Change_State(MOVE_BACK_JUMP);
		m_pSolider->Change_Animation(CSolider::JUMP_BACK, false);
	}
}

void CSolider_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	m_pSolider->Update_Attack_Time(fTimeDelta);
	switch (m_eState) {
	case IDLE:
		Idle(fTimeDelta);
		break;
	case MOVE:
		Move(fTimeDelta);
		break;
	case MOVE_LEFT:
		Move(fTimeDelta);
		break;
	case MOVE_RIGHT:
		Move(fTimeDelta);
		break;
	case MOVE_BACK:
		Move_Back(fTimeDelta);
		break;
	case MOVE_BACK_JUMP:
		Move_Back_Jump(fTimeDelta);
		break;
	case TURN:
		Turn(fTimeDelta);
		break;
	}
}

void CSolider_State_Idle::OnStateExit()
{
}

void CSolider_State_Idle::Idle(_float fTimeDelta)
{
	m_fDegree = m_pSolider->Cal_Degree_Target();

	if (m_fTimeDelta >= m_fStateTime) {
		if (abs(m_fDegree) > 30.f) {
			Change_State(TURN);
			if (m_fDegree >= 0)
				m_pSolider->Change_Animation(CSolider::TURN_LEFT, false);
			else
				m_pSolider->Change_Animation(CSolider::TURN_RIGHT, false);
		}
		else {	// Ÿ���� �ٶ󺸰� �ִٰ� �ǴܵǸ�
			_float fDegree_Start = m_pSolider->Cal_Degree_Start();
			if (abs(m_fDegree) < 45.f) {
				Change_State(MOVE);
				m_pSolider->Change_Animation(CSolider::MOVE_STRAIGHT_START, false);
			}
			else if (abs(m_fDegree) > 135.f) {
				Change_State(MOVE_BACK);
				m_pSolider->Change_Animation(CSolider::MOVE_BACK_START, false);
			}
			else {
				if (m_fDegree >= 0) {
					Change_State(MOVE_LEFT);
					m_pSolider->Change_Animation(CSolider::MOVE_LEFT_START, false);
				}
				else {
					Change_State(MOVE_RIGHT);
					m_pSolider->Change_Animation(CSolider::MOVE_RIGHT_START, false);
				}
			}
		}


	}

	if ((abs(m_fDegree) < 30.f) && m_pSolider->Get_AttackTime() >= 5.f)
		m_pSolider->Change_State(CSolider::STATE_ATTACK);

	if (m_pSolider->Is_Animation_Finished())
		m_pSolider->Change_Animation(CSolider::BATTLE_IDLE, true);

}

void CSolider_State_Idle::Move(_float fTimeDelta)
{
	m_pSolider->Get_Transform()->Go_Straight(fTimeDelta);

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pSolider->Get_CurrentTrackPosition()) == 21.f) {
			m_pSolider->Change_Animation(CSolider::MOVE_STRAIGHT_STOP_LEFT, false);
			Change_State(IDLE);
		}

		else if (round(m_pSolider->Get_CurrentTrackPosition()) == 41.f) {
			m_pSolider->Change_Animation(CSolider::MOVE_STRAIGHT_STOP_RIGHT, false);
			Change_State(IDLE);
		}
	}

	if (m_pSolider->Is_Animation_Finished()) {
		m_pSolider->Change_Animation(CSolider::MOVE_STRAIGHT_IDLE, true);
	}

}

void CSolider_State_Idle::Move_Left(_float fTimeDelta)
{
	m_pSolider->Get_Transform()->Go_Left(fTimeDelta);

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pSolider->Get_CurrentTrackPosition()) == 21.f) {
			m_pSolider->Change_Animation(CSolider::MOVE_LEFT_STOP_LEFT, false);
			Change_State(IDLE);
		}

		else if (round(m_pSolider->Get_CurrentTrackPosition()) == 41.f) {
			m_pSolider->Change_Animation(CSolider::MOVE_LEFT_STOP_RIGHT, false);
			Change_State(IDLE);
		}
	}

	if (m_pSolider->Is_Animation_Finished()) {
		m_pSolider->Change_Animation(CSolider::MOVE_LEFT_IDLE, true);
	}
}

void CSolider_State_Idle::Move_Right(_float fTimeDelta)
{
	m_pSolider->Get_Transform()->Go_Right(fTimeDelta);

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pSolider->Get_CurrentTrackPosition()) == 21.f) {
			m_pSolider->Change_Animation(CSolider::MOVE_RIGHT_STOP_LEFT, false);
			Change_State(IDLE);
		}

		else if (round(m_pSolider->Get_CurrentTrackPosition()) == 41.f) {
			m_pSolider->Change_Animation(CSolider::MOVE_RIGHT_STOP_RIGHT, false);
			Change_State(IDLE);
		}
	}

	if (m_pSolider->Is_Animation_Finished()) {
		m_pSolider->Change_Animation(CSolider::MOVE_RIGHT_IDLE, true);
	}
}

void CSolider_State_Idle::Move_Back(_float fTimeDelta)
{
	m_pSolider->Get_Transform()->Go_Backward(fTimeDelta);

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pSolider->Get_CurrentTrackPosition()) == 21.f) {
			m_pSolider->Change_Animation(CSolider::MOVE_BACK_STOP_LEFT, false);
			Change_State(IDLE);
		}

		else if (round(m_pSolider->Get_CurrentTrackPosition()) == 41.f) {
			m_pSolider->Change_Animation(CSolider::MOVE_BACK_STOP_RIGHT, false);
			Change_State(IDLE);
		}
	}

	if (m_pSolider->Is_Animation_Finished()) {
		m_pSolider->Change_Animation(CSolider::MOVE_BACK_IDLE, true);
	}
}

void CSolider_State_Idle::Move_Back_Jump(_float fTimeDelta)
{
	if (4.f <= m_pSolider->Get_CurrentTrackPosition() &&
		m_pSolider->Get_CurrentTrackPosition() <= 15.f)
		m_pSolider->Get_Transform()->Go_Backward(fTimeDelta * 5);

	if (m_pSolider->Is_Animation_Finished()) {
		Change_State(IDLE);
		m_pSolider->Change_Animation(CSolider::BATTLE_IDLE, true);
	}
}

void CSolider_State_Idle::Turn(_float fTimeDelta)
{
	_float t1 = EaseOutCublic(m_fPrevTimeDelta / 2.f);
	_float t2 = EaseOutCublic(m_fTimeDelta / 2.f);

	m_pSolider->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2 - t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 2.f) {
		Change_State(IDLE);
		m_pSolider->Change_Animation(CSolider::BATTLE_IDLE, true);
	}

	m_fPrevTimeDelta = m_fTimeDelta;
}

void CSolider_State_Idle::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
	m_fStateTime = Random_Float(2.f);
	m_fStateTime += 2.5f;
}

CSolider_State_Idle* CSolider_State_Idle::Create(CSolider* pSolider)
{
    CSolider_State_Idle* pInstance = new CSolider_State_Idle(pSolider);
    return pInstance;
}

void CSolider_State_Idle::Free()
{
    __super::Free();
}
