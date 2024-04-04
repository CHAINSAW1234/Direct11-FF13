#include "stdafx.h"
#include "Solider_State_Hit.h"
#include "Solider.h"

CSolider_State_Hit::CSolider_State_Hit(CSolider* pSolider)
{
    m_pSolider = pSolider;
}

void CSolider_State_Hit::OnStateEnter()
{
    m_pSolider->Change_Animation(CSolider::HIT, false);
}

void CSolider_State_Hit::OnStateUpdate(_float fTimeDelta)
{
    if (m_pSolider->Is_Animation_Finished())
        m_pSolider->Change_State(CSolider::STATE_IDLE);
}

void CSolider_State_Hit::OnStateExit()
{
}

CSolider_State_Hit* CSolider_State_Hit::Create(CSolider* pSolider)
{
    CSolider_State_Hit* pInstance = new CSolider_State_Hit(pSolider);
    return pInstance;
}

void CSolider_State_Hit::Free()
{
    __super::Free();
}
