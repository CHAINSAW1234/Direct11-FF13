#include "stdafx.h"
#include "Solider_State_Attack.h"
#include "Solider.h"


CSolider_State_Attack::CSolider_State_Attack(CSolider* pSolider)
{
    m_pSolider = pSolider;
}

void CSolider_State_Attack::OnStateEnter()
{
    m_eState = RUN;
    m_pSolider->Reset_Attakable();
    m_pSolider->Change_Animation(CSolider::RUN_START, false);
}

void CSolider_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    switch (m_eState) {
    case RUN:
        Run(fTimeDelta);
        break;
    case ATTACK:
        Attack(fTimeDelta);
        break;
    }
}

void CSolider_State_Attack::OnStateExit()
{
    m_pSolider->Set_IsJumpNext(true);
    m_pSolider->Reset_AttackTime();
}

void CSolider_State_Attack::Run(_float fTimeDelta)
{
    _float fDegree = m_pSolider->Cal_Degree_Target();
    _float fDist = m_pSolider->Cal_Dist_Target();

    if (abs(fDegree) > 5) {
        m_pSolider->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fDegree / (fDegree));
    }
    else {
        m_pSolider->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pSolider->Get_TargetPosition()));
    }


    if (fDist < .5f) {
        m_eState = ATTACK;
        m_pSolider->Change_Animation(CSolider::ANIMATION_SOLIDER(CSolider::ATTACK1 + rand()%3), false);
    }

    m_pSolider->Get_Transform()->Go_Straight(fTimeDelta * 3, m_pSolider->Get_Navigation());

    if (m_pSolider->Is_Animation_Finished())
        m_pSolider->Change_Animation(CSolider::RUN_IDLE, true);
}

void CSolider_State_Attack::Attack(_float fTimeDelta)
{
    m_pSolider->Check_Interact_Weapon();

    if (m_pSolider->Is_Animation_Finished())
        m_pSolider->Change_State(CSolider::STATE_IDLE);
}

CSolider_State_Attack* CSolider_State_Attack::Create(CSolider* pSolider)
{
    CSolider_State_Attack* pInstance = new CSolider_State_Attack(pSolider);

    return pInstance;
}

void CSolider_State_Attack::Free()
{
    __super::Free();
}
