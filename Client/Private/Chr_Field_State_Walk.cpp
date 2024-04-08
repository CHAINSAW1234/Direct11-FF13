#include "stdafx.h"
#include "Chr_Field_State_Walk.h"
#include "Chr_Field.h"

CChr_Field_State_Walk::CChr_Field_State_Walk(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Walk::OnStateEnter()
{

	// 1. 카메라의 방향 벡터와 키보드 입력을 이용해 이동 방향 정하기 
	m_vTargetDirection = m_pChr_Field->Cal_Target_Direction();
	// 1. Chr_Field의 Look벡터구하고, y값 없애서 정규화
	// 2. 키 입력 값으로 방향 결정, 그 방향과 Look백터로 각도 구하기
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(m_pChr_Field->Get_Look(), m_vTargetDirection);
	

	// 1. -30 ~ 30도는 단순 회전으로 처리
	if (abs(m_fDegree) < 45) {
		m_pChr_Field->Change_Animation(CChr_Field::WALK_IDLE, true);
		m_eState = MOVE;
	}
	else {
		// 각도가 0보다 크면 왼쪽이다
		if(m_fDegree >= 0)
			m_pChr_Field->Change_Animation(CChr_Field::WALK_START_WITH_TURN_RIGHT, false);
		else {
			m_pChr_Field->Change_Animation(CChr_Field::WALK_START_WITH_TURN_LEFT, false);
		}
		m_eState = TURN;
	}

}

void CChr_Field_State_Walk::OnStateUpdate(_float fTimeDelta)
{
	// 회전 여부 결정
	m_vTargetDirection = m_pChr_Field->Cal_Target_Direction();
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(m_pChr_Field->Get_Look(),m_vTargetDirection);

	switch (m_eState) {
	case MOVE:
		Move(fTimeDelta);
		break;
	case TURN:
		Turn(fTimeDelta);
		break;
	}

}

void CChr_Field_State_Walk::OnStateExit()
{
	// WALK_IDLE일거라고 가정
	if (m_pChr_Field->Get_CurrentTrackPosition() > 14.f && m_pChr_Field->Get_CurrentTrackPosition() <= 25.f) {
		m_pChr_Field->Change_Animation(CChr_Field::WALK_STOP_RIGHT, false);
	}
	else {
		m_pChr_Field->Change_Animation(CChr_Field::WALK_STOP_LEFT, false);
	}
	
}

void CChr_Field_State_Walk::Move(_float fTimeDelta)
{
	if (abs(m_fDegree) > 2.5f) {
		m_pChr_Field->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));
	}

	m_pChr_Field->Get_Transform()->Go_Straight(fTimeDelta, m_pChr_Field->Get_Navigation());
}

void CChr_Field_State_Walk::Turn(_float fTimeDelta)
{
	((CModel*)m_pChr_Field->Get_Component(g_strModelTag))->Play_Animation(fTimeDelta); // 도는 속도 2배로 하기 위한 특수 처리
	// 1.	
	if ((m_pChr_Field->Get_CurrentTrackPosition() >= 5.f) && (abs(m_fDegree) > 2.5f)) {
		m_pChr_Field->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * m_fDegree / abs(m_fDegree));
		m_pChr_Field->Get_Transform()->Go_Straight(fTimeDelta, m_pChr_Field->Get_Navigation());
	}

	if (m_pChr_Field->Get_CurrentTrackPosition() >= 21.f)
		m_pChr_Field->Get_Transform()->Go_Straight(fTimeDelta, m_pChr_Field->Get_Navigation());

	if (m_pChr_Field->Is_Animation_Finished()) {
		m_pChr_Field->Change_Animation(CChr_Field::WALK_IDLE, true);
		m_eState = MOVE;
	}

}

CChr_Field_State_Walk* CChr_Field_State_Walk::Create(CChr_Field* pChr_Field)
{
	CChr_Field_State_Walk* pInstance = new CChr_Field_State_Walk(pChr_Field);
	//m_pPlayer = pGameObject;

	return pInstance;
}

void CChr_Field_State_Walk::Free()
{
	__super::Free();
}
