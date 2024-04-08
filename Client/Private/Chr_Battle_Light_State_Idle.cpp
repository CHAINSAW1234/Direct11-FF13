#include "stdafx.h"
#include "Chr_Battle_Light_State_Idle.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_State_Idle::CChr_Battle_Light_State_Idle(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_State_Idle::OnStateEnter()
{
	m_iPatternCount = 0;

	Update_Hurt();

	// 체력이 부족한 경우
	if (m_isHurt) {
		Change_State(HURT);
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_START, false);
	}
	else {
		Change_State(IDLE);
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
	}
	m_eMovement = STRAIGHT;
}

void CChr_Battle_Light_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;

	m_pChr_Battle_Light->Determine_Action_Based_On_Command();
	m_pChr_Battle_Light->Update_ATB(fTimeDelta);

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

void CChr_Battle_Light_State_Idle::OnStateExit()
{
}

void CChr_Battle_Light_State_Idle::Idle(_float fTimeDelta)
{
	m_fDegree = m_pChr_Battle_Light->Cal_Degree_Target();

	if (m_fTimeDelta >= m_fStateTime) {
		if (abs(m_fDegree) > 45.f) {
			Change_State(TURN);
			if (m_fDegree >= 0)
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_TURN_LEFT, false);
			else
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_TURN_RIGHT, false);
		}
		else if (m_iPatternCount > 4 && m_pChr_Battle_Light->Cal_Dist_Start() <= 3.f) {
			m_fTimeDelta = 0.f;				// 그 자리에 가만히 
		}
		else {
			if (m_iPatternCount <= 4) {	// 랜덤 요소가 적용되는 이동 처리
				++m_iPatternCount;

				int irand = rand() % 4 + 1;
				m_eMovement = (MOVEMENT)(rand() % 4);	// 방향은 랜덤이오

				// 몬스터와 너무 가까우면 뒤로 가고, 너무 멀리있으면 앞으로 가야함
				_float fDist = m_pChr_Battle_Light->Cal_Dist_Target();
				if (fDist <= 3.f)
					m_eMovement = BACKWARD;
				else if (fDist >= 10.f)
					m_eMovement = STRAIGHT;
			}
			else {
				_float fDegree_Start = m_pChr_Battle_Light->Cal_Degree_Start();

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
			}
			Change_State(MOVE);
			Change_MovementAnimation();
		}
	}

	// 애니메이션 변경
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
		//m_pChr_Battle_Light->Change_Animation_Weapon(CChr_Battle_Light::WEAPON_OPEN_IDLE);		// 나중에 지울것
	}

	// 체력이 낮은 경우
	if (m_isHurt) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_START, false);
		Change_State(HURT);
	}
}

void CChr_Battle_Light_State_Idle::Hurt()
{
	m_fDegree = m_pChr_Battle_Light->Cal_Degree_Target();
	if (abs(m_fDegree) > 45.f) {
		Change_State(TURN);
		if (m_fDegree >= 0)
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_TURN_LEFT, false);
		else
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_TURN_RIGHT, false);
	}

	// 체력이 높으면 원상 복구됨
	if (!m_isHurt) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_END, false);
		m_eState = IDLE;
	}

	// 애니메이션 순차 재생
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		if (m_pChr_Battle_Light->Get_CurrentAnimationIndex() == CChr_Battle_Light::IDLE_HURT_START) {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT, true);
		}
	}
}

void CChr_Battle_Light_State_Idle::Move(_float fTimeDelta)
{
	m_fDegree = m_pChr_Battle_Light->Cal_Degree_Target();

	if (abs(m_fDegree) >= 30)
		m_isMoveTurn = true;

	if (m_isMoveTurn) {
		if (m_fDegree <= 5)
			m_isMoveTurn = false;
		m_pChr_Battle_Light->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));
	}


	if (m_fTimeDelta >= m_fStateTime) {
		if ((round(m_pChr_Battle_Light->Get_CurrentTrackPosition()) == 14.f) ||
			(round(m_pChr_Battle_Light->Get_CurrentTrackPosition())== 26.f)) {
			Change_State(IDLE);
			Change_MovementAnimation();
			return;
		}
	}

	// startposition으로 이동하는 상태에서의 Move임
	if (m_iPatternCount > 4) {
		_vector vLine = XMLoadFloat4(&Cal_Line());
		_vector vPos = m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION);
		if (XMVectorSum(vLine * vPos).m128_f32[0] <= 0.2f) {
			_float v = abs(Cal_Degree_From_Directions_Between_Min180_To_180(Cal_Dir_Start_To_Target(), Cal_Current_Direction()));
			if(45.f <= v && v <= 135.f )
				m_fTimeDelta = m_fStateTime;
		}
	}




	switch (m_eMovement)
	{
	case BACKWARD:
		if( 10.f <= m_pChr_Battle_Light->Cal_Dist_Target())
			m_fTimeDelta = m_fStateTime;
		m_pChr_Battle_Light->Get_Transform()->Go_Backward(fTimeDelta, m_pChr_Battle_Light->Get_Navigation());
		break;
	case LEFT:
		m_pChr_Battle_Light->Get_Transform()->Go_Left(fTimeDelta, m_pChr_Battle_Light->Get_Navigation());
		break;
	case RIGHT:
		m_pChr_Battle_Light->Get_Transform()->Go_Right(fTimeDelta, m_pChr_Battle_Light->Get_Navigation());
		break;
	case STRAIGHT:
		if ( 3.f >= m_pChr_Battle_Light->Cal_Dist_Target())
			m_fTimeDelta = m_fStateTime;
		m_pChr_Battle_Light->Get_Transform()->Go_Straight(fTimeDelta, m_pChr_Battle_Light->Get_Navigation());
		break;
	}

}

void CChr_Battle_Light_State_Idle::Turn(_float fTimeDelta)
{
	_float t1 = EaseOutCublic(m_fPrevTimeDelta);
	_float t2 = EaseOutCublic(m_fTimeDelta);

	m_pChr_Battle_Light->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2 - t1) * m_fDegree / 360);

	if (m_fTimeDelta >= 1.f) {
		Change_State(IDLE);
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
	}

	m_fPrevTimeDelta = m_fTimeDelta;
}

void CChr_Battle_Light_State_Idle::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
	m_fStateTime = Random_Float(2.f);
	m_fStateTime += 2.5f;
}

void CChr_Battle_Light_State_Idle::Change_MovementAnimation()
{
	switch (m_eState) {
	case MOVE:
		switch (m_eMovement) {
		case STRAIGHT:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_STRAIGHT, true);
			break;
		case BACKWARD:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_BACK, true);
			break;
		case LEFT:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_LEFT, true);
			break;
		case RIGHT:
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_RIGHT, true);
			break;
		}
		break;
	case IDLE: {
		_uint iCurrentAnimationIndex = m_pChr_Battle_Light->Get_CurrentAnimationIndex();
		if (round(m_pChr_Battle_Light->Get_CurrentTrackPosition()) == 14.f)
			iCurrentAnimationIndex += 1;
		else 
			iCurrentAnimationIndex += 2;

		m_pChr_Battle_Light->Change_Animation((CChr_Battle_Light::ANIMATION_CHR_BATTLE_LIGHT)iCurrentAnimationIndex, false);
		}
		 break;
	}
}

void CChr_Battle_Light_State_Idle::Update_Hurt()
{
	if ((_float)m_pChr_Battle_Light->Get_Hp() / (_float)m_pChr_Battle_Light->Get_MaxHp() <= 0.4) {
		m_isHurt = true;
	}
	else {
		m_isHurt = false;
	}
}

_float4 CChr_Battle_Light_State_Idle::Cal_Line()
{
	_vector vTargetPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position());
	_vector vStartPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Start_Position());
	// vLine : 몬스터와 시작 위치사이의 직선의 방향 벡터
	_vector vDir = XMVectorSetW(vTargetPosition - vStartPosition, 0.f);
	_float4 vLine;
	vLine.x = vDir.m128_f32[2];
	vLine.y = 0.f;
	vLine.z = -vDir.m128_f32[0];
	vLine.w = vDir.m128_f32[0] * vStartPosition.m128_f32[2] - vDir.m128_f32[2] * vStartPosition.m128_f32[0];

	return vLine;
}

_float4 CChr_Battle_Light_State_Idle::Cal_Dir_Start_To_Target()
{
	_vector vTargetPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position());
	_vector vStartPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Start_Position());
	// vLine : 몬스터와 시작 위치사이의 직선의 방향 벡터
	_vector vDir = XMVector3Normalize(XMVectorSetW(vTargetPosition - vStartPosition, 0.f));

	_float4 fDir;
	XMStoreFloat4(&fDir, vDir);
	return fDir;
}

_float4 CChr_Battle_Light_State_Idle::Cal_Current_Direction()
{
	_float4 vDir;
	switch (m_eMovement) {
	case STRAIGHT:
		XMStoreFloat4(&vDir, m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK));
		break;
	case BACKWARD:
		XMStoreFloat4(&vDir, -m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_LOOK));
		break;
	case RIGHT:
		XMStoreFloat4(&vDir, m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_RIGHT));
		break;
	case LEFT:
		XMStoreFloat4(&vDir, -m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_RIGHT));
		break;

	}

	return vDir;
}


CChr_Battle_Light_State_Idle* CChr_Battle_Light_State_Idle::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_State_Idle* pInstance = new CChr_Battle_Light_State_Idle(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_State_Idle::Free()
{
	__super::Free();
}
