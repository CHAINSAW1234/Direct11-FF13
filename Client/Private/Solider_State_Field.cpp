#include "stdafx.h"
#include "Solider_State_Field.h"
#include "Solider.h"


CSolider_State_Field::CSolider_State_Field(CSolider* pSolider)
{
    m_pSolider = pSolider;
}

void CSolider_State_Field::OnStateEnter()
{
    Change_State(IDLE);
    m_pSolider->Change_Animation(CSolider::FIELD_IDLE, true);
}

void CSolider_State_Field::OnStateUpdate(_float fTimeDelta)
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

void CSolider_State_Field::OnStateExit()
{
}

void CSolider_State_Field::Idle(_float fTimeDelta)
{


	if (m_fTimeDelta >= m_fStateTime) {

		if (rand() % 3) {
			m_fTimeDelta = 0.f;
			m_fStateTime = 2.f;
			return;
		}
		m_fDegree = Cal_Next_Degree();

		if (abs(m_fDegree >= 120)) {
			Change_State(TURN);
			if (m_fDegree >= 0)
				m_pSolider->Change_Animation(CSolider::FIELD_TURN_LEFT, false);
			else
				m_pSolider->Change_Animation(CSolider::FIELD_TURN_RIGHT, false);
		}
		else {
			Change_State(MOVE);
			m_pSolider->Change_Animation(CSolider::FIELD_MOVE_IDLE, true);
		}
		
	}

	if (m_pSolider->Is_Animation_Finished())
		m_pSolider->Change_Animation(CSolider::ANIMATION_SOLIDER(CSolider::FIELD_IDLE + rand()%3), true);
}

void CSolider_State_Field::Move(_float fTimeDelta)
{
	m_pSolider->Get_Transform()->Go_Straight(fTimeDelta, m_pSolider->Get_Navigation());

	if (m_fTimeDelta >= m_fStateTime) {
		if (round(m_pSolider->Get_CurrentTrackPosition()) == 13.f) {
			m_pSolider->Change_Animation(CSolider::FIELD_MOVE_STOP, false);
			Change_State(IDLE);
		}

	}

}

void CSolider_State_Field::Turn(_float fTimeDelta)
{
	_float t1 = EaseOutCublic(m_fPrevTimeDelta / 2.f);
	_float t2 = EaseOutCublic(m_fTimeDelta / 2.f);

	m_pSolider->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2 - t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 2.f) {
		Change_State(IDLE);
		m_pSolider->Change_Animation(CSolider::FIELD_IDLE, true);
	}

	m_fPrevTimeDelta = m_fTimeDelta;
}

void CSolider_State_Field::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
	m_fStateTime = Random_Float(2.f);
	m_fStateTime += 2.5f;
}

_float CSolider_State_Field::Cal_Next_Degree()
{
	_float4 vLook = m_pSolider->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);

	_float4 vRandPos;
	vRandPos.x = Random_Float(20.f);
	vRandPos.z = Random_Float(20.f);
	vRandPos.w = 1.f;

	_vector vDir_to_Start = XMLoadFloat4(&m_pSolider->Get_StartPosition()) + XMLoadFloat4(&vRandPos) - m_pSolider->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
	vDir_to_Start.m128_f32[1] = 0.f;
	vDir_to_Start.m128_f32[3] = 0.f;
	vDir_to_Start = XMVector3Normalize(vDir_to_Start);

	_float4 vTargetLook;
	XMStoreFloat4(&vTargetLook, vDir_to_Start);
	return Cal_Degree_From_Directions_Between_Min180_To_180(vLook, vTargetLook);
}

CSolider_State_Field* CSolider_State_Field::Create(CSolider* pSolider)
{
    CSolider_State_Field* pInstance = new CSolider_State_Field(pSolider);

    return pInstance;
}

void CSolider_State_Field::Free()
{
    __super::Free();
}
