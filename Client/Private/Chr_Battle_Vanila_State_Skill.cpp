#include "stdafx.h"
#include "Chr_Battle_Vanila_State_Skill.h"
#include "Chr_Battle_Vanila.h"
#include "UI_Skill.h"
#include "Effect.h"

CChr_Battle_Vanila_State_Skill::CChr_Battle_Vanila_State_Skill(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	m_pChr_Battle_Vanila = pChr_Battle_Vanila;
}

void CChr_Battle_Vanila_State_Skill::OnStateEnter()
{
	m_isCommandFinish = false;
	m_isCommandUse = false;
	m_pChr_Battle_Vanila->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Vanila->Get_Target_Position()));
	m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::ANIM_SKILL, false);

	CRole::SKILL eSkill = m_pChr_Battle_Vanila->Get_Current_Command();
	m_pUI_Skill = m_pChr_Battle_Vanila->Create_UI_Skill(eSkill);

	CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Effect_Skill.dat", m_pGameInstance, m_pChr_Battle_Vanila->Get_Device(), m_pChr_Battle_Vanila->Get_Context(), m_pChr_Battle_Vanila->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION));
}

void CChr_Battle_Vanila_State_Skill::OnStateUpdate(_float fTimeDelta)
{
	Skill();
}


void CChr_Battle_Vanila_State_Skill::OnStateExit()
{
	if (nullptr != m_pUI_Skill)
		m_pUI_Skill->Set_Dead(true);
	m_pUI_Skill = nullptr;
}

void CChr_Battle_Vanila_State_Skill::Skill()
{
	m_pChr_Battle_Vanila->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Vanila->Get_Target_Position()));

	if (!m_isCommandFinish) {

		if (!m_isCommandUse && m_pChr_Battle_Vanila->Get_CurrentTrackPosition() >= 35) {
			m_isCommandUse = true;
			m_pChr_Battle_Vanila->Create_Sphere(m_pChr_Battle_Vanila->Get_Attack_Magic());
			m_pChr_Battle_Vanila->Use_Command();
		}

		if (m_pChr_Battle_Vanila->Get_CurrentTrackPosition() >= 40) {
			CRole::SKILL eSkill = m_pChr_Battle_Vanila->Get_Current_Command();

			if (eSkill == CRole::SKILL_END) {
				m_isCommandFinish = true;
			}
			else {
				if (CChr_Battle_Vanila::ANIM_SKILL == m_pChr_Battle_Vanila->Get_CurrentAnimationIndex()) {
					m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::ANIM_SKILL2, false);
					CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Effect_Skill.dat", m_pGameInstance, m_pChr_Battle_Vanila->Get_Device(), m_pChr_Battle_Vanila->Get_Context(), m_pChr_Battle_Vanila->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION));
				}
				else {
					m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::ANIM_SKILL, false);
					CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Effect_Skill.dat", m_pGameInstance, m_pChr_Battle_Vanila->Get_Device(), m_pChr_Battle_Vanila->Get_Context(), m_pChr_Battle_Vanila->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION));
				}
				m_isCommandUse = false;
				m_pChr_Battle_Vanila->Set_TrackPosition(20.f);
			}
		}
	}
	else {
		if (m_pChr_Battle_Vanila->Is_Animation_Finished()) {
			m_pChr_Battle_Vanila->Change_State(CChr_Battle_Vanila::IDLE);
		}
	}
}

CChr_Battle_Vanila_State_Skill* CChr_Battle_Vanila_State_Skill::Create(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	CChr_Battle_Vanila_State_Skill* pInstance = new CChr_Battle_Vanila_State_Skill(pChr_Battle_Vanila);

	return pInstance;
}

void CChr_Battle_Vanila_State_Skill::Free()
{
	__super::Free();
}
