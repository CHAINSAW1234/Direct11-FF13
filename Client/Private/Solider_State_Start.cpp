#include "stdafx.h"
#include "Solider_State_Start.h"
#include "Solider.h"


CSolider_State_Start::CSolider_State_Start(CSolider* pSolider)
{
    m_pSolider = pSolider;
}

void CSolider_State_Start::OnStateEnter()
{
    m_pSolider->Change_Animation(CSolider::BATTLE_START, false);
}

void CSolider_State_Start::OnStateUpdate(_float fTimeDelta)
{
}

void CSolider_State_Start::OnStateExit()
{
}

CSolider_State_Start* CSolider_State_Start::Create(CSolider* pSolider)
{
    CSolider_State_Start* pInstance = new CSolider_State_Start(pSolider);
    return pInstance;
}

void CSolider_State_Start::Free()
{
    __super::Free();
}
