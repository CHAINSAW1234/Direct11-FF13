#include "stdafx.h"
#include "Warload_State_Idle.h"
#include "Warload.h"
#include "Chr_Battle.h"

CWarload_State_Idle::CWarload_State_Idle(CWarload* pWarload)
{
	m_pWarload = pWarload;
}

void CWarload_State_Idle::OnStateEnter()
{
	Change_State(IDLE);
	m_pWarload->Change_Animation(CWarload::BATTLE_IDLE, true);
	if (m_pWarload->Get_IsJumpNext()) {
		m_pWarload->Set_IsJumpNext(false);
		Change_State(MOVE_BACK_JUMP);
		m_pWarload->Change_Animation(CWarload::JUMP_BACK, false);
	}

}

void CWarload_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	m_pWarload->Update_Attack_Time(fTimeDelta);
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
	case MOVE_BACK_JUMP:
		Move_Back_Jump(fTimeDelta);
		break;
	case TURN:
		Turn(fTimeDelta);
		break;
	}
}

void CWarload_State_Idle::OnStateExit()
{
}

void CWarload_State_Idle::Idle(_float fTimeDelta)
{
	m_fDegree = m_pWarload->Cal_Degree_Target();

	if (m_fTimeDelta >= m_fStateTime) {
		if (abs(m_fDegree) < 30.f) {
			_float fDist = m_pWarload->Cal_Dist_Target();
			if (fDist <= 3.f) {
				Change_State(MOVE_BACK);
				m_pWarload->Change_Animation(CWarload::MOVE_STRAIGHT_IDLE, true);
			}
			else if (fDist >= 10.f) {
				Change_State(MOVE_BACK);
				m_pWarload->Change_Animation(CWarload::MOVE_BACK_IDLE, true);
			}
			else {
				if (rand() % 2) {
					Change_State(MOVE);
					m_pWarload->Change_Animation(CWarload::MOVE_STRAIGHT_IDLE, true);
				}
				else {
					Change_State(MOVE_BACK);
					m_pWarload->Change_Animation(CWarload::MOVE_BACK_IDLE, true);
				}
			}
			
		}
		else {
			Change_State(TURN);
			if (m_fDegree >= 0)
				m_pWarload->Change_Animation(CWarload::TURN_LEFT, false);
			else
				m_pWarload->Change_Animation(CWarload::TURN_RIGHT, false);
		}
	}

	if ((abs(m_fDegree) < 30.f) && m_pWarload->Get_AttackTime() >= 5.f)
		m_pWarload->Change_State(CWarload::STATE_ATTACK);

	if (m_pWarload->Is_Animation_Finished())
		m_pWarload->Change_Animation(CWarload::BATTLE_IDLE, true);

}

void CWarload_State_Idle::Move(_float fTimeDelta)
{
	m_pWarload->Get_Transform()->Go_Straight(fTimeDelta, m_pWarload->Get_Navigation());

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pWarload->Get_CurrentTrackPosition()) == 21.f) {
			m_pWarload->Change_Animation(CWarload::MOVE_STRAIGHT_STOP_LEFT, false);
			Change_State(IDLE);
		}

		else if (round(m_pWarload->Get_CurrentTrackPosition()) == 41.f) {
			m_pWarload->Change_Animation(CWarload::MOVE_STRAIGHT_STOP_RIGHT, false);
			Change_State(IDLE);
		}
	}

	if (3.f >= m_pWarload->Cal_Dist_Target())
		m_fTimeDelta = m_fStateTime;

}

void CWarload_State_Idle::Move_Back(_float fTimeDelta)
{
	m_pWarload->Get_Transform()->Go_Backward(fTimeDelta, m_pWarload->Get_Navigation());

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pWarload->Get_CurrentTrackPosition()) == 21.f) {
			m_pWarload->Change_Animation(CWarload::MOVE_BACK_STOP_LEFT, false);
			Change_State(IDLE);
		}

		else if (round(m_pWarload->Get_CurrentTrackPosition()) == 41.f) {
			m_pWarload->Change_Animation(CWarload::MOVE_BACK_STOP_RIGHT, false);
			Change_State(IDLE);
		}
	}

	if (10.f >= m_pWarload->Cal_Dist_Target())
		m_fTimeDelta = m_fStateTime;
}

void CWarload_State_Idle::Move_Back_Jump(_float fTimeDelta)
{
	if(4.f <= m_pWarload->Get_CurrentTrackPosition() &&
		m_pWarload->Get_CurrentTrackPosition() <= 15.f)
		m_pWarload->Get_Transform()->Go_Backward(fTimeDelta * 5, m_pWarload->Get_Navigation());

	if (m_pWarload->Is_Animation_Finished()) {
		Change_State(IDLE);
		m_pWarload->Change_Animation(CWarload::BATTLE_IDLE, true);
	}
}

void CWarload_State_Idle::Turn(_float fTimeDelta)
{
	_float t1 = (_float)EaseOutCublic(m_fPrevTimeDelta / 2.f);
	_float t2 = (_float)EaseOutCublic(m_fTimeDelta / 2.f);

	m_pWarload->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2 - t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 2.f) {
		Change_State(IDLE);
		m_pWarload->Change_Animation(CWarload::BATTLE_IDLE, true);
	}

	m_fPrevTimeDelta = m_fTimeDelta;
}

void CWarload_State_Idle::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
	m_fStateTime = Random_Float(2.f);
	m_fStateTime += 2.5f;
}

CWarload_State_Idle* CWarload_State_Idle::Create(CWarload* pWarload)
{
	CWarload_State_Idle* pInstance = new CWarload_State_Idle(pWarload);

	return pInstance;
}

void CWarload_State_Idle::Free()
{
	__super::Free();
}
