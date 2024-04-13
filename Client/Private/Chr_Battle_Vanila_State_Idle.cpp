#include "stdafx.h"
#include "Chr_Battle_Vanila_State_Idle.h"
#include "Chr_Battle_Vanila.h"

CChr_Battle_Vanila_State_Idle::CChr_Battle_Vanila_State_Idle(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	m_pChr_Battle_Vanila = pChr_Battle_Vanila;
}

void CChr_Battle_Vanila_State_Idle::OnStateEnter()
{
	Update_Hurt();

	if (m_isHurt) {
		m_eState = HURT;
		m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::HURT_START, false);
	}
	else {
		m_eState = IDLE;
		m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::ANIM_IDLE, true);
	}
	m_eMovement = STRAIGHT;
}

void CChr_Battle_Vanila_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	m_pChr_Battle_Vanila->Update_ATB(fTimeDelta);
	m_pChr_Battle_Vanila->Update_Command();
	Update_Hurt();

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

void CChr_Battle_Vanila_State_Idle::OnStateExit()
{
}

void CChr_Battle_Vanila_State_Idle::Idle(_float fTimeDelta)
{
	m_fDegree = m_pChr_Battle_Vanila->Cal_Degree_Target();

	if (m_pChr_Battle_Vanila->Get_CurrentAnimationIndex() == CChr_Battle_Vanila::ANIM_IDLE) {
		// 공격 가능시 공격으로 변경
		if (m_pChr_Battle_Vanila->Get_CommandCount() != 0 &&
			m_pChr_Battle_Vanila->Get_ATB() >= m_pChr_Battle_Vanila->Get_CommandCount()) {
			CRole::SKILL eSkill = m_pChr_Battle_Vanila->Get_Current_Command();

			switch (eSkill) {
			case CRole::CURE:
				m_pChr_Battle_Vanila->Change_State(CChr_Battle_Vanila::HEAL);
				break;
			case CRole::SKILL_END:
				break;
			default:
				m_pChr_Battle_Vanila->Change_State(CChr_Battle_Vanila::SKILL);
				break;
			}
		}

		if (m_fTimeDelta >= m_fStateTime) {
			if (abs(m_fDegree) > 30.f) {
				Change_State(TURN);
				if (m_fDegree > 0)
					m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::TURN_LEFT, false);
				else
					m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::TURN_RIGHT, false);
			}
			else if (m_pChr_Battle_Vanila->Cal_Dist_Start() <= 3.f) {
				;
			}
			else {
				_float fDegree_Start = m_pChr_Battle_Vanila->Cal_Degree_Start();

				if (abs(fDegree_Start) < 45.f)
					m_eMovement = STRAIGHT;
				else if (abs(fDegree_Start) > 135.f)
					m_eMovement = BACKWARD;
				else {
					if (fDegree_Start >= 0)
						m_eMovement = RIGHT;
					else
						m_eMovement = LEFT;
				}
				Change_State(MOVE);
				Change_MovementAnimation();
			}
		}
	}

	if (m_pChr_Battle_Vanila->Is_Animation_Finished()) {
		m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::ANIM_IDLE, true);
	}

	if (m_isHurt) {
		m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::HURT_START, false);
		Change_State(HURT);
	}
}

void CChr_Battle_Vanila_State_Idle::Hurt()
{
	m_fDegree = m_pChr_Battle_Vanila->Cal_Degree_Target();
	if (abs(m_fDegree) > 45.f) {
		Change_State(TURN);
		if (m_fDegree >= 0)
			m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::TURN_LEFT, false);
		else
			m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::TURN_RIGHT, false);
	}

	// 체력이 높으면 원상 복구됨
	if (!m_isHurt) {
		m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::HURT_END, false);
		m_eState = IDLE;
	}

	// 애니메이션 순차 재생
	if (m_pChr_Battle_Vanila->Is_Animation_Finished()) {
		if (m_pChr_Battle_Vanila->Get_CurrentAnimationIndex() == CChr_Battle_Vanila::HURT_START) {
			m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::HURT_IDLE, true);
		}
	}
}

void CChr_Battle_Vanila_State_Idle::Move(_float fTimeDelta)
{
	m_fDegree = m_pChr_Battle_Vanila->Cal_Degree_Target();

	if (m_fTimeDelta >= m_fStateTime) {
		if ((round(m_pChr_Battle_Vanila->Get_CurrentTrackPosition()) == 14.f) ||
			(round(m_pChr_Battle_Vanila->Get_CurrentTrackPosition()) == 26.f)) {
			Change_State(IDLE);
			Change_MovementAnimation();
			return;
		}
	}
	// startpoint에 멀어지지 않도록 처리
	_vector vLine = XMLoadFloat4(&Cal_Line());
	_vector vPos = m_pChr_Battle_Vanila->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
	if (XMVectorSum(vLine * vPos).m128_f32[0] <= 0.2f) {
		_float v = abs(Cal_Degree_From_Directions_Between_Min180_To_180(Cal_Dir_Start_To_Target(), Cal_Current_Direction()));
		if (45.f <= v && v <= 135.f)
			m_fTimeDelta = m_fStateTime;
	}

	switch (m_eMovement)
	{
	case BACKWARD:
		if (10.f <= m_pChr_Battle_Vanila->Cal_Dist_Target())
			m_fTimeDelta = m_fStateTime;
		m_pChr_Battle_Vanila->Get_Transform()->Go_Backward(fTimeDelta, m_pChr_Battle_Vanila->Get_Navigation());
		break;
	case LEFT:
		m_pChr_Battle_Vanila->Get_Transform()->Go_Left(fTimeDelta, m_pChr_Battle_Vanila->Get_Navigation());
		break;
	case RIGHT:
		m_pChr_Battle_Vanila->Get_Transform()->Go_Right(fTimeDelta, m_pChr_Battle_Vanila->Get_Navigation());
		break;
	case STRAIGHT:
		if (3.f >= m_pChr_Battle_Vanila->Cal_Dist_Target())
			m_fTimeDelta = m_fStateTime;
		m_pChr_Battle_Vanila->Get_Transform()->Go_Straight(fTimeDelta, m_pChr_Battle_Vanila->Get_Navigation());
		break;
	}
}

void CChr_Battle_Vanila_State_Idle::Turn(_float fTimeDelta)
{
	_float t1 = (_float)EaseOutCublic(m_fPrevTimeDelta);
	_float t2 = (_float)EaseOutCublic(m_fTimeDelta);

	m_pChr_Battle_Vanila->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2 - t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 1.f) {
		Change_State(IDLE);
		m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::ANIM_IDLE, true);
	}

	m_fPrevTimeDelta = m_fTimeDelta;
}

void CChr_Battle_Vanila_State_Idle::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
	m_fStateTime = Random_Float(2.f);
	m_fStateTime += 2.5f;
}

void CChr_Battle_Vanila_State_Idle::Change_MovementAnimation()
{
	switch (m_eState) {
	case MOVE:
		switch (m_eMovement) {
		case STRAIGHT:
			m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::MOVE_STRAIGHT_IDLE, true);
			break;
		case BACKWARD:
			m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::MOVE_BACK_IDLE, true);
			break;
		case LEFT:
			m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::MOVE_LEFT_IDLE, true);
			break;
		case RIGHT:
			m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::MOVE_RIGHT_IDLE, true);
			break;
		}
		break;
	case IDLE: {
		_uint iCurrentAnimationIndex = m_pChr_Battle_Vanila->Get_CurrentAnimationIndex();
		if (round(m_pChr_Battle_Vanila->Get_CurrentTrackPosition()) == 14.f)
			iCurrentAnimationIndex += 1;
		else
			iCurrentAnimationIndex += 2;

		m_pChr_Battle_Vanila->Change_Animation((CChr_Battle_Vanila::ANIMATION_CHR_BATTLE_VANILA)iCurrentAnimationIndex, false);
	}
			 break;
	}

}

void CChr_Battle_Vanila_State_Idle::Update_Hurt()
{
	if ((_float)m_pChr_Battle_Vanila->Get_Hp() / (_float)m_pChr_Battle_Vanila->Get_MaxHp() <= 0.4) {
		m_isHurt = true;
	}
	else {
		m_isHurt = false;
	}
}

_float4 CChr_Battle_Vanila_State_Idle::Cal_Line()
{
	_vector vTargetPosition = XMLoadFloat4(&m_pChr_Battle_Vanila->Get_Target_Position());
	_vector vStartPosition = XMLoadFloat4(&m_pChr_Battle_Vanila->Get_Start_Position());
	// vLine : 몬스터와 시작 위치사이의 직선의 방향 벡터
	_vector vDir = XMVectorSetW(vTargetPosition - vStartPosition, 0.f);
	_float4 vLine;
	vLine.x = vDir.m128_f32[2];
	vLine.y = 0.f;
	vLine.z = -vDir.m128_f32[0];
	vLine.w = vDir.m128_f32[0] * vStartPosition.m128_f32[2] - vDir.m128_f32[2] * vStartPosition.m128_f32[0];

	return vLine;
}

_float4 CChr_Battle_Vanila_State_Idle::Cal_Dir_Start_To_Target()
{
	_vector vTargetPosition = XMLoadFloat4(&m_pChr_Battle_Vanila->Get_Target_Position());
	_vector vStartPosition = XMLoadFloat4(&m_pChr_Battle_Vanila->Get_Start_Position());
	// vLine : 몬스터와 시작 위치사이의 직선의 방향 벡터
	_vector vDir = XMVector3Normalize(XMVectorSetW(vTargetPosition - vStartPosition, 0.f));

	_float4 fDir;
	XMStoreFloat4(&fDir, vDir);
	return fDir;
}

_float4 CChr_Battle_Vanila_State_Idle::Cal_Current_Direction()
{
	_float4 vDir;
	switch (m_eMovement) {
	case STRAIGHT:
		XMStoreFloat4(&vDir, m_pChr_Battle_Vanila->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK));
		break;
	case BACKWARD:
		XMStoreFloat4(&vDir, -m_pChr_Battle_Vanila->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK));
		break;
	case RIGHT:
		XMStoreFloat4(&vDir, m_pChr_Battle_Vanila->Get_Transform()->Get_State_Vector(CTransform::STATE_RIGHT));
		break;
	case LEFT:
		XMStoreFloat4(&vDir, -m_pChr_Battle_Vanila->Get_Transform()->Get_State_Vector(CTransform::STATE_RIGHT));
		break;
	}

	return vDir;
}

CChr_Battle_Vanila_State_Idle* CChr_Battle_Vanila_State_Idle::Create(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	CChr_Battle_Vanila_State_Idle* pInstance = new CChr_Battle_Vanila_State_Idle(pChr_Battle_Vanila);

	return pInstance;
}

void CChr_Battle_Vanila_State_Idle::Free()
{
	__super::Free();
}
