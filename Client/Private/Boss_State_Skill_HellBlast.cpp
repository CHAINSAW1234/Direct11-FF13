#include "stdafx.h"
#include "Boss_State_Skill_HellBlast.h"
#include "Boss.h"
#include "UI_Skill.h"

CBoss_State_Skill_HellBlast::CBoss_State_Skill_HellBlast(CBoss* pBoss)
{
    m_pBoss = pBoss;
}

void CBoss_State_Skill_HellBlast::OnStateEnter()
{
    m_eState = MOVE;
    m_pBoss->Change_Animation(CBoss::MOVE_STRAIGHT_START, false);
    m_pUI_Skill = m_pBoss->Create_UI_Skill(TEXT("헬 블래스트"));
}

void CBoss_State_Skill_HellBlast::OnStateUpdate(_float fTimeDelta)
{
    // 충돌 체크 함수 
    switch (m_eState) {
    case MOVE:
        Move(fTimeDelta);
        break;
    case SKILL:
        Skill(fTimeDelta);
        break;
    }
}

void CBoss_State_Skill_HellBlast::OnStateExit()
{
    if (nullptr != m_pUI_Skill)
        m_pUI_Skill->Set_Dead(true);
    m_pUI_Skill = nullptr;
}

void CBoss_State_Skill_HellBlast::Move(_float fTimeDelta)
{
    _float fDist = m_pBoss->Cal_Dist_Start();

    m_pBoss->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pBoss->Get_StartPosition()));

    if (fDist <= 0.5) {
        m_eState = SKILL;
        m_pBoss->Change_Animation(CBoss::SKILL_HELLBLAST, false);
    }

    m_pBoss->Get_Transform()->Go_Straight(fTimeDelta * 5, m_pBoss->Get_Navigation());

    if (m_pBoss->Is_Animation_Finished())
        m_pBoss->Change_Animation(CBoss::MOVE_STRAIGHT_IDLE, true);
}

void CBoss_State_Skill_HellBlast::Skill(_float fTimeDelta)
{
    if (40.f <= m_pBoss->Get_CurrentTrackPosition() &&
        m_pBoss->Get_CurrentTrackPosition() <= 50.f)
        m_pBoss->Get_Transform()->Go_Up(fTimeDelta * 2);

    if (150.f <= m_pBoss->Get_CurrentTrackPosition() &&
        m_pBoss->Get_CurrentTrackPosition() <= 160.f)
        m_pBoss->Get_Transform()->Go_Down(fTimeDelta * 2);

    if (m_pBoss->Is_Animation_Finished())
        m_pBoss->Change_State(CBoss::STATE_IDLE);
}

CBoss_State_Skill_HellBlast* CBoss_State_Skill_HellBlast::Create(CBoss* pBoss)
{
    CBoss_State_Skill_HellBlast* pInstance = new CBoss_State_Skill_HellBlast(pBoss);
    return pInstance;
}

void CBoss_State_Skill_HellBlast::Free()
{
    __super::Free();
}
