#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Idle.h"
#include "Chr_Battle_Sazh.h"

CChr_Battle_Sazh_State_Idle::CChr_Battle_Sazh_State_Idle(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Idle::OnStateEnter()
{
	if (m_isHurt) {
		m_eState = HURT;
		m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::HURT_START, true);
	}
	else {
		m_eState = IDLE;
		m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::ANIM_IDLE, true);
	}
	m_eMovement = STRAIGHT;
}

void CChr_Battle_Sazh_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_pChr_Battle_Sazh->Update_ATB(fTimeDelta);


	// 공격 가능시 공격으로 변경
	if (m_pChr_Battle_Sazh->Get_CommandCount() !=0 &&
		m_pChr_Battle_Sazh->Get_ATB() >= m_pChr_Battle_Sazh->Get_CommandCount()) {
		CRole::SKILL eSkill = m_pChr_Battle_Sazh->Get_Current_Command();

		switch (eSkill) {
		case CRole::FLAMEBLOW:
			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::ATTACK);
			break;
		case CRole::SKILL_END:
			break;
		default:
			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::SKILL);
			break;
		}
	}

	switch (m_eState) {
	case IDLE:
		Idle(fTimeDelta);
		break;
	case HURT:
		Hurt();
		break;
	case MOVE:
		Move(fTimeDelta);
		break;
	case TURN:
		Turn(fTimeDelta);
		break;
	}


}

void CChr_Battle_Sazh_State_Idle::OnStateExit()
{
}

void CChr_Battle_Sazh_State_Idle::Idle(_float fTimeDelta)
{
	if (abs(m_fDegree) > 30.f) {
		m_eState = TURN;
		if (m_fDegree > 0)
			m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::TURN_LEFT, false);
		else
			m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::TURN_RIGHT, false);
	}

	// startposition과 너무 많이 차이나면 Move로 변경

}

void CChr_Battle_Sazh_State_Idle::Hurt()
{
}

void CChr_Battle_Sazh_State_Idle::Move(_float fTimeDelta)
{
}

void CChr_Battle_Sazh_State_Idle::Turn(_float fTimeDelta)
{
	if (abs(m_fDegree) < 3.f) {
		m_pChr_Battle_Sazh->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Sazh->Get_Target_Position()));
	}
	else {
		m_pChr_Battle_Sazh->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));
	}

	if (m_pChr_Battle_Sazh->Is_Animation_Finished()) {
		m_eState = IDLE;
		m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::ANIM_IDLE, true);
	}

}

void CChr_Battle_Sazh_State_Idle::Update_LookAt()
{
	_float4 vTargetLook;
	_vector vCurrent_To_Target_Look = (XMLoadFloat4(&m_pChr_Battle_Sazh->Get_Target_Position())
		- m_pChr_Battle_Sazh->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION));
	vCurrent_To_Target_Look.m128_f32[1] = 0.f;
	XMStoreFloat4(&vTargetLook, XMVectorSetW(XMVector3Normalize(vCurrent_To_Target_Look), 0.f));

	_float4 vCurrentLook = m_pChr_Battle_Sazh->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);

	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vCurrentLook, vTargetLook);
}

CChr_Battle_Sazh_State_Idle* CChr_Battle_Sazh_State_Idle::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	CChr_Battle_Sazh_State_Idle* pInstance = new CChr_Battle_Sazh_State_Idle(pChr_Battle_Sazh);

	return pInstance;
}

void CChr_Battle_Sazh_State_Idle::Free()
{
	__super::Free();
}
