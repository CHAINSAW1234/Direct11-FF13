#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Attack.h"
#include "Chr_Battle_Sazh.h"

CChr_Battle_Sazh_State_Attack::CChr_Battle_Sazh_State_Attack(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Attack::OnStateEnter()
{
	m_isCommandFinish = false;
	m_pChr_Battle_Sazh->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Sazh->Get_Target_Position()));
	m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::ATTACK_NOR1, false);
}

void CChr_Battle_Sazh_State_Attack::OnStateUpdate(_float fTimeDelta)
{
	Attack();
}

void CChr_Battle_Sazh_State_Attack::OnStateExit()
{
}

void CChr_Battle_Sazh_State_Attack::Attack()
{
	if (!m_isCommandFinish) {
		if (m_pChr_Battle_Sazh->Get_CurrentTrackPosition() >= 25) {
			m_pChr_Battle_Sazh->Use_Command();
			CRole::SKILL eSkill = m_pChr_Battle_Sazh->Get_Current_Command();

			if (eSkill == CRole::FLAMEBLOW) {
				m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::ANIMATION_CHR_BATTLE_SAZH(m_pChr_Battle_Sazh->Get_CurrentAnimationIndex() + 1), false);
			}
			else {
				m_isCommandFinish = true;
			}
		}
		
	}
	else {
		if (m_pChr_Battle_Sazh->Is_Animation_Finished()) {
			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::IDLE);
		}
	}
}

CChr_Battle_Sazh_State_Attack* CChr_Battle_Sazh_State_Attack::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	CChr_Battle_Sazh_State_Attack* pInstance = new CChr_Battle_Sazh_State_Attack(pChr_Battle_Sazh);

	return pInstance;
}

void CChr_Battle_Sazh_State_Attack::Free()
{
	__super::Free();
}
