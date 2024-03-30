#include "stdafx.h"
#include "Leopard_State_Idle.h"
#include "Leopard.h"

CLeopard_State_Idle::CLeopard_State_Idle(CLeopard* pLeopard)
{
	m_pLeopard = pLeopard;
}

void CLeopard_State_Idle::OnStateEnter()
{
	m_fTimeDelta = 0.f;
	m_eState = IDLE;
	m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);
}

void CLeopard_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;

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
	// 타겟과의 거리 유지하기?
	//
	Update_LookAt();

	if (abs(m_fDegree) < 45.f) {
		m_eState = MOVE;
		m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_START, false);
		m_fTimeDelta = 0.f;
	}
	else {
		m_fTimeDelta = 0.f;
		_int special = rand() % 3;
		if (0 == special) {			// 매우 낮은 확률로 나오게 처리함
			m_eState = TURN;
			if (m_fDegree >= 0) {
				m_pLeopard->Change_Animation(CLeopard::TURN_LEFT, false);
			}
			else {
				m_pLeopard->Change_Animation(CLeopard::TURN_RIGHT, false);
			}
		}
		else {
			m_eState = MOVE_BACK;
			m_pLeopard->Change_Animation(CLeopard::MOVE_BACK_START, false);
		}
		//m_pLeopard->Set_RotationPerSec(XMConvertToRadians((abs(m_fDegree))));


	}

}

void CLeopard_State_Idle::Move(_float fTimeDelta)
{
	//m_pLeopard->Get_Transform()->Go_Straight(fTimeDelta);

	if (m_fTimeDelta >= 1.f) {
		m_eState = IDLE;
		m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);
		m_fTimeDelta = 0.f;
	}

	if (m_pLeopard->Is_Animation_Finished()) {
		m_pLeopard->Change_Animation(CLeopard::MOVE_STRAIGHT_IDLE, true);
	}
}

void CLeopard_State_Idle::Move_Back(_float fTimeDelta)
{
	//m_pLeopard->Get_Transform()->Go_Backward(fTimeDelta);

	if (m_fTimeDelta >= 1.f) {
		m_eState = IDLE;
		m_fTimeDelta = 0.f;
		m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);

	}

	if (m_pLeopard->Is_Animation_Finished()) {
		m_pLeopard->Change_Animation(CLeopard::MOVE_BACK_IDLE, true);
	}

}

void CLeopard_State_Idle::Turn(_float fTimeDelta)
{
	_float t1 = EaseOutCublic(m_fPrevTimeDelta/2);
	_float t2 = EaseOutCublic(m_fTimeDelta/2);

	m_pLeopard->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2-t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 2.f) {
		m_eState = IDLE;
		m_fTimeDelta = 0.f;
		m_pLeopard->Change_Animation(CLeopard::BATTLE_IDLE, true);
		m_fPrevTimeDelta = 0.f;
		m_fTimeDelta = 0.f;
	}
	m_fPrevTimeDelta = m_fTimeDelta;
}

void CLeopard_State_Idle::Update_LookAt()
{
	_float4 vLook = m_pLeopard->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);
	_vector vDir_to_Start = XMLoadFloat4(&m_pLeopard->Get_StartPosition()) - m_pLeopard->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
	vDir_to_Start.m128_f32[1] = 0.f;
	vDir_to_Start = XMVector3Normalize(vDir_to_Start);

	_float4 vTargetLook;
	XMStoreFloat4(&vTargetLook, vDir_to_Start);
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vLook, vTargetLook);
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
