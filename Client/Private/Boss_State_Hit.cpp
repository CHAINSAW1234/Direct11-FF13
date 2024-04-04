#include "stdafx.h"
#include "Boss_State_Hit.h"
#include "Boss.h"

CBoss_State_Hit::CBoss_State_Hit(CBoss* pBoss)
{
    m_pBoss = pBoss;
}

void CBoss_State_Hit::OnStateEnter()
{
}

void CBoss_State_Hit::OnStateUpdate(_float fTimeDelta)
{
    if (m_pBoss->Is_Animation_Finished())
        m_pBoss->Change_State(CBoss::STATE_IDLE);
}

void CBoss_State_Hit::OnStateExit()
{
}

CBoss_State_Hit* CBoss_State_Hit::Create(CBoss* pBoss)
{
    CBoss_State_Hit* pInstance = new CBoss_State_Hit(pBoss);
    return pInstance;
}

void CBoss_State_Hit::Free()
{
    __super::Free();
}
