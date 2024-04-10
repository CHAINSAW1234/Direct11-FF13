#include "stdafx.h"
#include "Boss_State_Skill_Barrier.h"
#include "Boss.h"
#include "UI_Skill.h"

CBoss_State_Skill_Barrier::CBoss_State_Skill_Barrier(CBoss* pBoss)
{
    m_pBoss = pBoss;
}

void CBoss_State_Skill_Barrier::OnStateEnter()
{
    m_pBoss->Change_Animation(CBoss::SKILL_BARRIER, false);
    m_isSkill = false;
    m_pUI_Skill = m_pBoss->Create_UI_Skill(TEXT("º£¸®¾î"));
}

void CBoss_State_Skill_Barrier::OnStateUpdate(_float fTimeDelta)
{
    Skill();

}

void CBoss_State_Skill_Barrier::OnStateExit()
{
    if (nullptr != m_pUI_Skill)
        m_pUI_Skill->Set_Dead(true);
    m_pUI_Skill = nullptr;
}

void CBoss_State_Skill_Barrier::Skill()
{
    if (!m_isSkill && m_pBoss->Get_CurrentTrackPosition() >= 50.f) {
        m_isSkill = true;
        m_pBoss->Set_Barrier(true);
    }

    if (m_pBoss->Is_Animation_Finished())
        m_pBoss->Change_State(CBoss::STATE_IDLE);
}

CBoss_State_Skill_Barrier* CBoss_State_Skill_Barrier::Create(CBoss* pBoss)
{
    CBoss_State_Skill_Barrier* pInstance = new CBoss_State_Skill_Barrier(pBoss);
    return pInstance;
}

void CBoss_State_Skill_Barrier::Free()
{
    __super::Free();
}
