#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Skill.h"
#include "Chr_Battle_Sazh.h"

CChr_Battle_Sazh_State_Skill::CChr_Battle_Sazh_State_Skill(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Skill::OnStateEnter()
{
	m_isCommandFinish = false;
	m_pChr_Battle_Sazh->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Sazh->Get_Target_Position()));
	m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::SKILL_NOR1, false);
}

void CChr_Battle_Sazh_State_Skill::OnStateUpdate(_float fTimeDelta)
{
	Skill();
}

void CChr_Battle_Sazh_State_Skill::OnStateExit()
{
}

void CChr_Battle_Sazh_State_Skill::Skill()
{
	if (!m_isCommandFinish) {
		if (m_pChr_Battle_Sazh->Get_CurrentTrackPosition() >= 40) {
			m_pChr_Battle_Sazh->Use_Command();
			CRole::SKILL eSkill = m_pChr_Battle_Sazh->Get_Current_Command();

			if (eSkill == CRole::SKILL_END) {
				m_isCommandFinish = true;
			}
			else {
				if (CChr_Battle_Sazh::SKILL_NOR1 == m_pChr_Battle_Sazh->Get_CurrentAnimationIndex())
					m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::SKILL_NOR2, false);
				else
					m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::SKILL_NOR1, false);
				m_pChr_Battle_Sazh->Set_TrackPosition(20.f);
			}
		}
	}
	else {
		if (m_pChr_Battle_Sazh->Is_Animation_Finished()) {
			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::IDLE);
		}
	}
}

CChr_Battle_Sazh_State_Skill* CChr_Battle_Sazh_State_Skill::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	CChr_Battle_Sazh_State_Skill* pInstance = new CChr_Battle_Sazh_State_Skill(pChr_Battle_Sazh);

	return pInstance;
}

void CChr_Battle_Sazh_State_Skill::Free()
{
	__super::Free();
}
