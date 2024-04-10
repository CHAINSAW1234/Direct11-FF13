#include "stdafx.h"
#include "Chr_Battle_Vanila_State_Heal.h"
#include "Chr_Battle_Vanila.h"
#include "UI_Skill.h"

CChr_Battle_Vanila_State_Heal::CChr_Battle_Vanila_State_Heal(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	m_pChr_Battle_Vanila = pChr_Battle_Vanila;
}

void CChr_Battle_Vanila_State_Heal::OnStateEnter()
{
	m_isCommandFinish = false;
	if (!XMVector3Equal(m_pChr_Battle_Vanila->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION), XMLoadFloat4(&m_pChr_Battle_Vanila->Get_Target_Position())))
		m_pChr_Battle_Vanila->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Vanila->Get_Target_Position()));
	m_pChr_Battle_Vanila->Change_Animation(CChr_Battle_Vanila::ANIM_HEAL, false);

	CRole::SKILL eSkill = m_pChr_Battle_Vanila->Get_Current_Command();
	m_pUI_Skill = m_pChr_Battle_Vanila->Create_UI_Skill(eSkill);
}

void CChr_Battle_Vanila_State_Heal::OnStateUpdate(_float fTimeDelta)
{
	Heal();
}

void CChr_Battle_Vanila_State_Heal::OnStateExit()
{
	if (nullptr != m_pUI_Skill)
		m_pUI_Skill->Set_Dead(true);
	m_pUI_Skill = nullptr;
}

void CChr_Battle_Vanila_State_Heal::Heal()
{
	if (!m_isCommandFinish) {
		if (m_pChr_Battle_Vanila->Get_CurrentTrackPosition() >= 20) {
			m_pChr_Battle_Vanila->Use_Command();
			m_isCommandFinish = true;
		}
	}
	else {
		if (m_pChr_Battle_Vanila->Is_Animation_Finished()) {
			m_pChr_Battle_Vanila->Change_State(CChr_Battle_Vanila::IDLE);
		}
	}
}

CChr_Battle_Vanila_State_Heal* CChr_Battle_Vanila_State_Heal::Create(CChr_Battle_Vanila* pChr_Battle_Vanila)
{
	CChr_Battle_Vanila_State_Heal* pInstance = new CChr_Battle_Vanila_State_Heal(pChr_Battle_Vanila);

	return pInstance;
}

void CChr_Battle_Vanila_State_Heal::Free()
{
	__super::Free();
	Safe_Release(m_pUI_Skill);
}
