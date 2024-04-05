#include "stdafx.h"
#include "Chr_Field_State_Move.h"
#include "Chr_Field.h"

CChr_Field_State_Move::CChr_Field_State_Move(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Move::OnStateEnter()
{
	// 1. ī�޶��� ���� ���Ϳ� Ű���� �Է��� �̿��� �̵� ���� ���ϱ� 
	m_vTargetDirection = m_pChr_Field->Cal_Target_Direction();
	// 1. Chr_Field�� Look���ͱ��ϰ�, y�� ���ּ� ����ȭ
	// 2. Ű �Է� ������ ���� ����, �� ����� Look���ͷ� ���� ���ϱ�
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(m_pChr_Field->Get_Look(), m_vTargetDirection);

	m_eState = START;
	// 1. -30 ~ 30���� �ܼ� ȸ������ ó��
	if (abs(m_fDegree) <= 30)
		m_pChr_Field->Change_Animation(CChr_Field::MOVE_START, false);
	else {
		// ������ 0���� ũ�� �����̴�
		if (m_fDegree >= 0)
			m_pChr_Field->Change_Animation(CChr_Field::MOVE_START_WITH_TURN_LEFT, false);
		else
			m_pChr_Field->Change_Animation(CChr_Field::MOVE_START_WITH_TURN_RIGHT, false);
	}
}

void CChr_Field_State_Move::OnStateUpdate(_float fTimeDelta)
{
	// ȸ�� ���� ����
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
	// MOVE_IDLE�ϰŶ�� ����
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
