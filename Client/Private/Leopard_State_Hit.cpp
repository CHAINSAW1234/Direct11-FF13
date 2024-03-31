#include "stdafx.h"
#include "Leopard_State_Hit.h"
#include "Leopard.h"

CLeopard_State_Hit::CLeopard_State_Hit(CLeopard* pLeopard)
{
    m_pLeopard = pLeopard;
}

void CLeopard_State_Hit::OnStateEnter()
{
    if (m_pLeopard->Get_CurrentAnimationIndex() == CLeopard::RUN_IDLE)
        m_pLeopard->Change_Animation(CLeopard::HIT_RUN, false);
    else  
        m_pLeopard->Change_Animation(CLeopard::HIT, false);
}

void CLeopard_State_Hit::OnStateUpdate(_float fTimeDelta)
{
    if (m_pLeopard->Is_Animation_Finished())
        m_pLeopard->Change_State(CLeopard::STATE_IDLE);
}

void CLeopard_State_Hit::OnStateExit()
{
}

CLeopard_State_Hit* CLeopard_State_Hit::Create(CLeopard* pLeopard)
{
    CLeopard_State_Hit* pInstance = new CLeopard_State_Hit(pLeopard);
    return pInstance;
}

void CLeopard_State_Hit::Free()
{
    __super::Free();
}
