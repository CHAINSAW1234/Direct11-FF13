#include "stdafx.h"
#include "Chr_Field_State_Walk.h"
#include "Chr_Field.h"

CChr_Field_State_Walk::CChr_Field_State_Walk(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Walk::OnStateEnter()
{

	// 1. ī�޶��� ���� ���Ϳ� Ű���� �Է��� �̿��� �̵� ���� ���ϱ� 
	m_vTargetDirection = m_pChr_Field->Cal_Target_Direction();
	// 1. Chr_Field�� Look���ͱ��ϰ�, y�� ���ּ� ����ȭ
	// 2. Ű �Է� ������ ���� ����, �� ����� Look���ͷ� ���� ���ϱ�
	m_fDegree = Cal_Degree_From_Directions_Between_Min180_To_180(m_pChr_Field->Get_Look(), m_vTargetDirection);
	

	// 1. -30 ~ 30���� �ܼ� ȸ������ ó��
	if (abs(m_fDegree) < 45) {
		m_pChr_Field->Change_Animation(CChr_Field::WALK_IDLE, true);
		m_eState = MOVE;
	}
	else {
		// ������ 0���� ũ�� �����̴�
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
	// ȸ�� ���� ����
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
	// WALK_IDLE�ϰŶ�� ����
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
	((CModel*)m_pChr_Field->Get_Component(g_strModelTag))->Play_Animation(fTimeDelta); // ���� �ӵ� 2��� �ϱ� ���� Ư�� ó��
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
