#include "stdafx.h"
#include "Leopard_State_Field.h"
#include "Leopard.h"
#include "Chr_Battle.h"

CLeopard_State_Field::CLeopard_State_Field(CLeopard* pLeopard)
{
	m_pLeopard = pLeopard;
}

void CLeopard_State_Field::OnStateEnter()
{
	Change_State(IDLE);
	m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);
}

void CLeopard_State_Field::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	switch (m_eState) {
	case IDLE:
		Idle(fTimeDelta);
		break;
	case MOVE:
		Move(fTimeDelta);
		break;
	case TURN:
		Turn(fTimeDelta);
		break;
	}
}

void CLeopard_State_Field::OnStateExit()
{
}

void CLeopard_State_Field::Idle(_float fTimeDelta)
{
	if (m_fTimeDelta >= m_fStateTime) {
		if (m_ePrevState == TURN) {
			Change_State(MOVE);
			m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_START, false);
		}
		else {
			m_fDegree = Cal_Next_Degree();
			Change_State(TURN);
			if (m_fDegree >= 0)
				m_pLeopard->Change_Animation(CLeopard::TURN_LEFT, false);
			else
				m_pLeopard->Change_Animation(CLeopard::TURN_RIGHT, false);
		}
	}

	if (m_pLeopard->Is_Animation_Finished())
		m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);
}

void CLeopard_State_Field::Move(_float fTimeDelta)
{
	m_pLeopard->Get_Transform()->Go_Straight(fTimeDelta);

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

	if (m_pLeopard->Is_Animation_Finished()) {
		m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_IDLE, true);
	}
}

void CLeopard_State_Field::Turn(_float fTimeDelta)
{
	_float t1 = EaseOutCublic(m_fPrevTimeDelta / 2);
	_float t2 = EaseOutCublic(m_fTimeDelta / 2);

	m_pLeopard->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2 - t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 2.f) {
		Change_State(IDLE);
		m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);
	}

	m_fPrevTimeDelta = m_fTimeDelta;
}

void CLeopard_State_Field::Change_State(STATE eState)
{
	m_ePrevState = m_eState;
	m_eState = eState;
	m_fTimeDelta = 0.f;
	m_fStateTime = Random_Float(2.f);
	m_fStateTime += 2.5f;
}

_float CLeopard_State_Field::Cal_Next_Degree()
{
	_float4 vLook = m_pLeopard->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);


	_float4 vRandPos;
	vRandPos.x = Random_Float(20.f);
	vRandPos.z = Random_Float(20.f);
	vRandPos.w = 1.f;

	_vector vDir_to_Start = XMLoadFloat4(&m_pLeopard->Get_StartPosition()) + XMLoadFloat4(&vRandPos) - m_pLeopard->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
	vDir_to_Start.m128_f32[1] = 0.f;
	vDir_to_Start.m128_f32[3] = 0.f;
	vDir_to_Start = XMVector3Normalize(vDir_to_Start);

	_float4 vTargetLook;
	XMStoreFloat4(&vTargetLook, vDir_to_Start);
	return Cal_Degree_From_Directions_Between_Min180_To_180(vLook, vTargetLook);
}

CLeopard_State_Field* CLeopard_State_Field::Create(CLeopard* pLeopard)
{
	CLeopard_State_Field* pInstance = new CLeopard_State_Field(pLeopard);

	return pInstance;
}

void CLeopard_State_Field::Free()
{
	__super::Free();
}
