#include "stdafx.h"
#include "Chr_Battle_Light_State_Idle.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_State_Idle::CChr_Battle_Light_State_Idle(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_State_Idle::OnStateEnter()
{
	// ü���� ������ ���
	if (m_isHurt) {
		m_eState = HURT;
	}
	else {
		m_eState = IDLE;
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
	}
	m_isPatternEnable = true;		// ��� �ʿ��غ��̱� �ѵ� ����� ���� ����
	m_eMovement = STRAIGHT;
}

void CChr_Battle_Light_State_Idle::OnStateUpdate(_float fTimeDelta)
{
	m_pChr_Battle_Light->Determine_Action_Based_On_Command();
	m_pChr_Battle_Light->Update_ATB(fTimeDelta);

	m_fDegree = m_pChr_Battle_Light->Cal_Degree_Target();

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

void CChr_Battle_Light_State_Idle::OnStateExit()
{
}

void CChr_Battle_Light_State_Idle::Idle(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;
	// Turn üũ
	if (m_fTurnCheckTimeDelta >= 2.f) {		//m_fTurnCheckTimeDelt�� �� ƽ���� ������
		if (abs(m_fDegree) > 5.f) {
			m_pChr_Battle_Light->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));
		}
		if (m_fTurnCheckTimeDelta >= 3.f)
			m_fTurnCheckTimeDelta = 0.f;
	}

	// Movement üũ
	if (m_fTimeDelta >= 2.f) {
		m_fTimeDelta = 0.f;
		if (m_isPatternEnable) {	// Startposition���� �̵��ϴ� ����
			m_eState = MOVE;
			Change_MovementAnimation();
		}
		else {	// StartPosition�� ��ġ�ϴ� ��� 
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

	// �ִϸ��̼� ����
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
		//m_pChr_Battle_Light->Change_Animation_Weapon(CChr_Battle_Light::WEAPON_OPEN_IDLE);		// ���߿� �����
	}
	// ü���� ���� ���
	if (m_isHurt) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_START, false);
		m_eState = HURT;
	}


}

void CChr_Battle_Light_State_Idle::Hurt()
{
	// ü���� ������ ���� ������
	if (!m_isHurt) {
		m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT_END, false);
		m_eState = IDLE;
	}

	// �ִϸ��̼� ���� ���
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		if (m_pChr_Battle_Light->Get_CurrentAnimationIndex() == CChr_Battle_Light::IDLE_HURT_START) {
			m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::IDLE_HURT, true);
		}
	}

	
}

void CChr_Battle_Light_State_Idle::Move(_float fTimeDelta)
{
	m_fMoveTimeDelta += fTimeDelta;

	// ���� �ֱ⸶�� ������ ���� �ʿ���
	if (m_fTurnCheckTimeDelta >= 2.f) {		//m_fTurnCheckTimeDelt�� �� ƽ���� ������
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
	// vLine : ���Ϳ� ���� ��ġ������ ������ ���� ����
	_vector vLine = XMVectorSetW(vStartPosition - vTargetPosition, 0.f);
	vLine.m128_f32[1] = 0;
	vLine = XMVector3Normalize(vLine);

	switch (m_eMovement) {
		// STRAIGHT, BACKWARD�� ������ ��ġ ���� �̵�
	case STRAIGHT: {
		_float fDist = XMVector3Length(vTargetPosition - vCurrentPosition).m128_f32[0];
		m_pChr_Battle_Light->Get_Transform()->Go_Straight(fTimeDelta, m_pChr_Battle_Light->Get_Navigation());
		if (fDist <= m_fDist) {
			Update_Movement();
		}
		break;
	}
	case BACKWARD: {
		_float fDist = XMVector3Length(vTargetPosition - vCurrentPosition).m128_f32[0];
		m_pChr_Battle_Light->Get_Transform()->Go_Backward(fTimeDelta, m_pChr_Battle_Light->Get_Navigation());
		if (fDist >= m_fDist) {
			Update_Movement();
		}
		break;
	}
	// Next�� ���, Target�� Start ������ ������ ������ �� ���� �̵� 
	case NEXT: {
		_int iSide = Check_Movement_Next(vTargetPosition, vLine, vCurrentPosition);
		if (iSide == 0) {	// NEXT �̵� ��
			Update_Movement();
		}
		else if (iSide == 1) { // ������
			m_pChr_Battle_Light->Get_Transform()->Go_Right(fTimeDelta, m_pChr_Battle_Light->Get_Navigation());
		}
		else {
			m_pChr_Battle_Light->Get_Transform()->Go_Left(fTimeDelta, m_pChr_Battle_Light->Get_Navigation());
		}
		break;
	}
	// TO_START�� ���, StartPosition���� �̵�
	case TO_START: {
		_float fDist = XMVector3Length(vStartPosition - vCurrentPosition).m128_f32[0];
		m_pChr_Battle_Light->Get_Transform()->Move_To_Target(vStartPosition, fTimeDelta, 0.f, m_pChr_Battle_Light->Get_Navigation());
		if (fDist <= 1) {
			Update_Movement();
		}
		break;
	}

	}

}

void CChr_Battle_Light_State_Idle::Turn(_float fTimeDelta)
{
	//_float t1 = EaseOutCublic(m_fPrevTimeDelta / 1.5f);
	//_float t2 = EaseOutCublic(m_fTimeDelta / 1.5f);

	//m_pChr_Battle_Light->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), (t2 - t1) * m_fDegree / 360);

	//if (m_fTimeDelta >= 1.5f) {
	//	Change_State(IDLE);
	//	m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_IDLE, true);
	//}

	//m_fPrevTimeDelta = m_fTimeDelta;
}

void CChr_Battle_Light_State_Idle::Change_State(STATE eState)
{
	//m_eState = eState;
	//m_fTimeDelta = 0.f;
	//m_fStateTime = Random_Float(2.f);
	//m_fStateTime += 2.5f;
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
		case NEXT: {
			if (m_iNextDir == -1)
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_LEFT, true);
			else
				m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::MOVE_RIGHT, true);
			}
			break;
		case TO_START: {
			// ���� ���ؼ� �ִϸ��̼� �����ؾߵ�
			_float fDegree = m_pChr_Battle_Light->Cal_Degree_Start();

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

void CChr_Battle_Light_State_Idle::Update_Movement()
{	
	// ���� �̵��� ����Ǿ����� ȣ���ϴ� �Լ�
	// ���� �ൿ�� �����ؼ� �����ϰ�, ������ MOVE STATE�� �������� �� �ൿ�� ������ ��

	_float4 vTargetPosition = m_pChr_Battle_Light->Get_Target_Position();
	_float4 vPosition = m_pChr_Battle_Light->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION);
	_float fDist = XMVector4Length(XMLoadFloat4(&vTargetPosition) - XMLoadFloat4(&vPosition)).m128_f32[0];	// ���� �Ÿ� ����
	
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

_int CChr_Battle_Light_State_Idle::Check_Movement_Next(FXMVECTOR vTargetPos, FXMVECTOR vLineDir, FXMVECTOR vCurPos)
{
	// Next �̵��϶� ������ �÷��̾��� ��ġ�� �浹, ���� �÷��̾��� ��, �� ��ġ �Ǻ� �Լ�
	XMVECTOR VminusP0 = XMVectorSubtract(vCurPos, vTargetPos);
	XMVECTOR vCross = XMVector3Cross(VminusP0, vLineDir);

	// Z���� ���� (DirectX������ 3D ���͸� �ַ� �ٷ�Ƿ�, 2D ��Ȳ �����Ͽ� Z������ ���)
	_float YComponent = XMVectorGetY(vCross);

	if ((XMVectorGetX(XMVector3Length(vCross)) / XMVectorGetX(XMVector3Length(vLineDir))) < 1)
		return 0;

	if (YComponent > 0) {
		m_iNextDir = -1;
		return -1; // ����
	}

	m_iNextDir = 1;
	return 1; // ������
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
