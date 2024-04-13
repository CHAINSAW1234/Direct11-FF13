#include "stdafx.h"
#include "Solider_Gun_State_Attack.h"
#include "Solider_Gun.h"

CSolider_Gun_State_Attack::CSolider_Gun_State_Attack(CSolider_Gun* pSolider_Gun)
{
    m_pSolider_Gun = pSolider_Gun;
}

void CSolider_Gun_State_Attack::OnStateEnter()
{
    m_pSolider_Gun->Change_Animation(CSolider::ATTACK3, false);
}

void CSolider_Gun_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    Attack(fTimeDelta);
}

void CSolider_Gun_State_Attack::OnStateExit()
{
    m_pSolider_Gun->Reset_AttackTime();
    m_pSolider_Gun->Change_Target();
}

void CSolider_Gun_State_Attack::Attack(_float fTimeDelta)
{
    if (m_pSolider_Gun->Is_Animation_Finished()) {
        if (m_pSolider_Gun->Get_CurrentAnimationIndex() == CSolider::ATTACK1)
            m_pSolider_Gun->Change_State(CSolider::STATE_IDLE);
        else if (m_pSolider_Gun->Get_CurrentAnimationIndex() == CSolider::ATTACK2) {
            m_pSolider_Gun->Create_Bullet();

            m_pSolider_Gun->Change_Animation(CSolider::ATTACK1, false);
        }
        else
            m_pSolider_Gun->Change_Animation(CSolider::ATTACK2, false);
    }
}

CSolider_Gun_State_Attack* CSolider_Gun_State_Attack::Create(CSolider_Gun* pSolider_Gun)
{
    CSolider_Gun_State_Attack* pInstance = new CSolider_Gun_State_Attack(pSolider_Gun);

    return pInstance;
}

void CSolider_Gun_State_Attack::Free()
{
    __super::Free();
}
