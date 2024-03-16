#include "stdafx.h"
#include "Chr_Battle_Light_Idle.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_Idle::CChr_Battle_Light_Idle(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_Idle::OnStateEnter()
{

	// 체력이 부족한 경우
	if (m_isHurt) {
		m_eState = HURT;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_START, false);
	}
	else {
		m_eState = IDLE;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
	}
	m_isPatternEnable = true;		// 제어가 필요해보이긴 한데 방법이 아직 없다
}

void CChr_Battle_Light_Idle::OnStateUpdate(_float fTimeDelta)
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_I)) {
		m_isHurt = !m_isHurt;
	}

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_P)) {
		m_eState = IDLE;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_PREPARE, false);
		m_fTimeDelta = 0.f;

	}

	Update_LookAt();

	m_fTurnCheckTimeDelta += fTimeDelta;

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
	}

}

void CChr_Battle_Light_Idle::OnStateExit()
{
}

void CChr_Battle_Light_Idle::Idle(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	// Turn 체크
	if (m_fTurnCheckTimeDelta >= 2.f) {		//m_fTurnCheckTimeDelt는 매 틱마다 더해짐
		if (abs(m_fDegree) > 30.f) {
			m_pChr_Battle_Light->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));
		}
		if (m_fTurnCheckTimeDelta >= 3.f)
			m_fTurnCheckTimeDelta = 0.f;
	}

	// Movement 체크
	if (m_fTimeDelta >= 2.f) {
		m_fTimeDelta = 0.f;
		if (m_isPatternEnable) {	// Startposition으로 이동하는 과정
			m_eState = MOVE;
			Change_MovementAnimation();
		}
		else {	// StartPosition에 위치하는 경우 
			_vector vStartPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Start_Position());
			_vector vCurrentPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION));
			_float fDist = XMVector3Length(vStartPosition - vCurrentPosition).m128_f32[0];
			if (fDist >= 2) {
				m_eState = MOVE;
				m_eMovement = TO_START;
				Change_MovementAnimation();
			}
		}
	}

	// 애니메이션 변경
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
	}
	// 체력이 낮은 경우
	if (m_isHurt) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_START, false);
		m_eState = HURT;
	}


}

void CChr_Battle_Light_Idle::Hurt()
{
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

void CChr_Battle_Light_Idle::Move(_float fTimeDelta)
{
	m_fMoveTimeDelta += fTimeDelta;

	// 일정 주기마다 돌리기 
	if (m_fTurnCheckTimeDelta >= 2.f) {		//m_fTurnCheckTimeDelt는 매 틱마다 더해짐
		if (abs(m_fDegree) > 30.f) {
			m_pChr_Battle_Light->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));
			if (m_fTurnCheckTimeDelta >= 3.f)
				m_fTurnCheckTimeDelta = 0.f;
			return;
		}
	}
	
	if (m_fMoveTimeDelta >= 4.f) {
		m_eState = IDLE;
		Change_MovementAnimation();
		m_fMoveTimeDelta = 0.f;
		return;
	}

	_vector vTargetPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position());
	_vector vStartPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Start_Position());
	_vector vCurrentPosition = XMLoadFloat4(&m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION));
	// vLine : 몬스터와 시작 위치사이의 직선의 방향 벡터
	_vector vLine = XMVectorSetW(vStartPosition - vTargetPosition, 0.f);

	switch (m_eMovement) {
		// STRAIGHT, BACKWARD는 지정한 위치 까지 이동
	case STRAIGHT: {
		_float fDist = XMVector3Length(vTargetPosition - vCurrentPosition).m128_f32[0];
		m_pChr_Battle_Light->Get_Transform()->Go_Straight(fTimeDelta);
		if (fDist <= m_fDist) {
			Update_Movement();
		}
		break;
	}
	case BACKWARD: {
		_float fDist = XMVector3Length(vTargetPosition - vCurrentPosition).m128_f32[0];
		m_pChr_Battle_Light->Get_Transform()->Go_Backward(fTimeDelta);
		if (fDist >= m_fDist) {
			Update_Movement();
		}
		break;
	}
	// Next인 경우, Target과 Start 사이의 직선에 도달할 때 까지 이동 
	case NEXT: {
		_int iSide = Check_Movement_Next(vTargetPosition, vLine, vCurrentPosition);
		if (iSide == 0) {	// NEXT 이동 끝
			Update_Movement();
		}
		else if (iSide == 1) { // 오른쪽
			m_pChr_Battle_Light->Get_Transform()->Go_Right(fTimeDelta);
		}
		else {
			m_pChr_Battle_Light->Get_Transform()->Go_Left(fTimeDelta);
		}
		break;
	}
	// TO_START인 경우, StartPosition으로 이동
	case TO_START: {
		_float fDist = XMVector3Length(vStartPosition - vCurrentPosition).m128_f32[0];
		m_pChr_Battle_Light->Get_Transform()->Move_To_Target(vStartPosition, fTimeDelta);
		if (fDist <= 1) {
			Update_Movement();
		}
		break;
	}

	}

}

void CChr_Battle_Light_Idle::Change_MovementAnimation()
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
		case NEXT: {
			if (m_iNextDir == -1)
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_LEFT, true);
			else
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_RIGHT, true);
			}
			break;
		case TO_START: {
			// 방향 구해서 애니메이션 결정해야됨
			_float4 vCurrent_To_Target_Look;
			_float4 vCurrent_To_Start_Look;
			XMStoreFloat4(&vCurrent_To_Target_Look,
				XMVector3Normalize((XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position())
					- m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION))));
			XMStoreFloat4(&vCurrent_To_Start_Look,
				XMVector3Normalize((XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position())
					- XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position()))));

			_float fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vCurrent_To_Start_Look, vCurrent_To_Target_Look);

			if (abs(fDegree) < 60)
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_STRAIGHT, true);
			else if(abs(fDegree) > 150)
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_BACK, true);
			else if(fDegree > 0)
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_LEFT, true);
			else 
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_RIGHT, true);
			}
			break;
		}
		break;
	case IDLE: {
		_uint iCurrentAnimationIndex = m_pChr_Battle_Light->Get_CurrentAnimationIndex();
		if (m_pChr_Battle_Light->Get_CurrentTrackPosition() <= 5.f ||
			m_pChr_Battle_Light->Get_CurrentTrackPosition() >= 16.f)
			iCurrentAnimationIndex += 1;
		else
			iCurrentAnimationIndex += 2;
		m_pChr_Battle_Light->Change_Animation((CChr_Battle_Light::ANIMATION_CHR_BATTLE_LIGHT)iCurrentAnimationIndex, false);
		}
		 break;
	}
}

void CChr_Battle_Light_Idle::Update_Movement()
{	
	// 현재 이동이 종료되었을때 호출하는 함수
	// 다음 행동을 결정해서 저장하고, 다음번 MOVE STATE에 들어왔을때 그 행동을 수행할 것

	_float4 vTargetPosition = m_pChr_Battle_Light->Get_Target_Position();
	_float4 vPosition = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION);
	_float fDist = XMVector4Length(XMLoadFloat4(&vTargetPosition) - XMLoadFloat4(&vPosition)).m128_f32[0];	// 현재 거리 차이
	
	m_fTimeDelta = 0.f;
	m_eState = IDLE;
	Change_MovementAnimation();
	switch (m_eMovement) {
	case STRAIGHT:
		m_fDist = fDist + 10;
		m_eMovement = BACKWARD;
		break;
	case BACKWARD:
		if (rand() % 3) {
			m_fDist = 5;
			m_eMovement = STRAIGHT;
		}
		else {
			m_eMovement = NEXT;
		}
		break;
	case NEXT:
		m_eMovement = TO_START;
		break;
	case TO_START:
		m_isPatternEnable = false;
		break;
	}

}

void CChr_Battle_Light_Idle::Update_LookAt()
{
	_float4 vTargetLook;
	_vector vCurrent_To_Target_Look = (XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position())
		- m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION));
	vCurrent_To_Target_Look.m128_f32[1] = 0.f;
	XMStoreFloat4(&vTargetLook, XMVectorSetW(XMVector3Normalize(vCurrent_To_Target_Look), 0.f));

	_float4 vCurrentLook = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_LOOK);
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(vCurrentLook, vTargetLook);
}

_int CChr_Battle_Light_Idle::Check_Movement_Next(FXMVECTOR vTargetPos, FXMVECTOR vLineDir, FXMVECTOR vCurPos)
{
	// Next 이동일때 직선과 플레이어의 위치의 충돌, 현재 플레이어의 좌, 우 위치 판별 함수
	XMVECTOR VminusP0 = XMVectorSubtract(vCurPos, vTargetPos);
	XMVECTOR vCross = XMVector3Cross(VminusP0, vLineDir);

	// Z성분 추출 (DirectX에서는 3D 벡터를 주로 다루므로, 2D 상황 가정하에 Z성분을 사용)
	_float YComponent = XMVectorGetY(vCross);

	if ((XMVectorGetX(XMVector3Length(vCross)) / XMVectorGetX(XMVector3Length(vLineDir))) < 1)
		return 0;

	if (YComponent > 0) {
		m_iNextDir = -1;
		return -1; // 왼쪽
	}

	m_iNextDir = 1;
	return 1; // 오른쪽
}

CChr_Battle_Light_Idle* CChr_Battle_Light_Idle::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Idle* pInstance = new CChr_Battle_Light_Idle(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_Idle::Free()
{
	__super::Free();
}
