#include "stdafx.h"
#include "Chr_Field_State_Move.h"
#include "Chr_Field.h"

CChr_Field_State_Move::CChr_Field_State_Move(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Move::OnStateEnter()
{
	// 1. 카메라의 방향 벡터와 키보드 입력을 이용해 이동 방향 정하기 
	m_vTargetDirection = m_pChr_Field->Cal_Target_Direction();
	// 1. Chr_Field의 Look벡터구하고, y값 없애서 정규화
	// 2. 키 입력 값으로 방향 결정, 그 방향과 Look백터로 각도 구하기
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(m_pChr_Field->Get_Look(), m_vTargetDirection);

	m_eState = START;
	// 1. -30 ~ 30도는 단순 회전으로 처리
	if (abs(m_fDegree) <= 30)
		m_pChr_Field->Change_Animation(CChr_Field::MOVE_START, false);
	else {
		// 각도가 0보다 크면 왼쪽이다
		if (m_fDegree >= 0)
			m_pChr_Field->Change_Animation(CChr_Field::MOVE_START_WITH_TURN_LEFT, false);
		else
			m_pChr_Field->Change_Animation(CChr_Field::MOVE_START_WITH_TURN_RIGHT, false);
	}
}

void CChr_Field_State_Move::OnStateUpdate(_float fTimeDelta)
{
	// 회전 여부 결정
	m_vTargetDirection = m_pChr_Field->Cal_Target_Direction();
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(m_pChr_Field->Get_Look(), m_vTargetDirection);


	switch (m_eState) {
	case START:
		Start(fTimeDelta);
		break;
	case MOVE:
		Move(fTimeDelta);
		break;
	case TURN:
		Drift(fTimeDelta);
		break;
	}
}

void CChr_Field_State_Move::OnStateExit()
{
	// MOVE_IDLE일거라고 가정
	if (m_pChr_Field->Get_CurrentTrackPosition() > 7.f && m_pChr_Field->Get_CurrentTrackPosition() <= 15.f)
		m_pChr_Field->Change_Animation(CChr_Field::MOVE_STOP_LEFT, false);
	else
		m_pChr_Field->Change_Animation(CChr_Field::MOVE_STOP_RIGHT, false);

}

void CChr_Field_State_Move::Start(_float fTimeDelta)
{
	if (abs(m_fDegree) > 5)
		m_pChr_Field->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));


	if (m_pChr_Field->Is_Animation_Finished()) {
		m_pChr_Field->Change_Animation(CChr_Field::MOVE_IDLE, true);
		m_eState = MOVE;
	}

}

void CChr_Field_State_Move::Move(_float fTimeDelta)
{
	if (abs(m_fDegree) > 5)
		m_pChr_Field->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));

	m_pChr_Field->Get_Transform()->Go_Straight(fTimeDelta * 3, m_pChr_Field->Get_Navigation());

	if (m_fDegree > 90) {
		m_pChr_Field->Change_Animation(CChr_Field::MOVE_TURN_LEFT, false);
		m_eState = TURN;
	}

	if (m_fDegree < -90) {
		m_pChr_Field->Change_Animation(CChr_Field::MOVE_TURN_RIGHT, false);
		m_eState = TURN;
	}

}

void CChr_Field_State_Move::Drift(_float fTimeDelta)
{
	if (m_pChr_Field->Get_CurrentTrackPosition() < 7.f)
		m_pChr_Field->Get_Transform()->Go_Straight(fTimeDelta * 2, m_pChr_Field->Get_Navigation());

	if ((m_pChr_Field->Get_CurrentTrackPosition() > 7.f) && (abs(m_fDegree) > 5.f))
		m_pChr_Field->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));

	if (m_pChr_Field->Is_Animation_Finished()) {
		m_pChr_Field->Change_Animation(CChr_Field::MOVE_IDLE, true);
		m_eState = MOVE;
	}

}

CChr_Field_State_Move* CChr_Field_State_Move::Create(CChr_Field* pChr_Field)
{
	CChr_Field_State_Move* pInstance = new CChr_Field_State_Move(pChr_Field);
	//m_pPlayer = pGameObject;

	return pInstance;
}

void CChr_Field_State_Move::Free()
{
	__super::Free();
}
