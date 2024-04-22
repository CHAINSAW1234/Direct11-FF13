#include "stdafx.h"
#include "Boss_State_Field.h"
#include "Boss.h"

CBoss_State_Field::CBoss_State_Field(CBoss* pBoss)
{
    m_pBoss = pBoss;
}

void CBoss_State_Field::OnStateEnter()
{
    m_pBoss->Get_Transform()->Look_At_ForLandObject({ 0.f, 0.f, -0.001f,1.f });

    m_pBoss->Change_Animation(CBoss::IDLE, true);
}

void CBoss_State_Field::OnStateUpdate(_float fTimeDelta)
{
}

void CBoss_State_Field::OnStateExit()
{
}

CBoss_State_Field* CBoss_State_Field::Create(CBoss* pBoss)
{
    CBoss_State_Field* pInstance = new CBoss_State_Field(pBoss);
    return pInstance;
}

void CBoss_State_Field::Free()
{
    __super::Free();
}
