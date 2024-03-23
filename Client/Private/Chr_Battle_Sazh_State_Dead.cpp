#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Dead.h"
#include "Chr_Battle_Sazh.h"

CChr_Battle_Sazh_State_Dead::CChr_Battle_Sazh_State_Dead(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Dead::OnStateEnter()
{
	// �ִϸ��̼��� �����ϸ� �ȵ�
	// ��Ʈ�� ����� ���� ���¿� ���� �ٸ� �ִϸ��̼��� ���޵�
}

void CChr_Battle_Sazh_State_Dead::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Sazh->Is_Animation_Finished()) {
		if (CChr_Battle_Sazh::DEAD_END == m_pChr_Battle_Sazh->Get_CurrentAnimationIndex())
			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::IDLE);
		else {
			m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::DEAD_IDLE, true);
		}
	}

	if (m_pChr_Battle_Sazh->Get_Hp() > 0) {
		m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::DEAD_END, false);
	}

}

void CChr_Battle_Sazh_State_Dead::OnStateExit()
{
}

CChr_Battle_Sazh_State_Dead* CChr_Battle_Sazh_State_Dead::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	CChr_Battle_Sazh_State_Dead* pInstance = new CChr_Battle_Sazh_State_Dead(pChr_Battle_Sazh);

	return pInstance;
}

void CChr_Battle_Sazh_State_Dead::Free()
{
	__super::Free();
}
