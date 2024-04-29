#include "stdafx.h"
#include "Boss_State_Attack_Magic.h"
#include "Boss.h"

CBoss_State_Attack_Magic::CBoss_State_Attack_Magic(CBoss* pBoss)
{
    m_pBoss = pBoss;
}

void CBoss_State_Attack_Magic::OnStateEnter()
{
    m_pBoss->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pBoss->Get_TargetPosition()));
    m_pBoss->Change_Animation(CBoss::ATTACK2, false);
    m_isAttack = false;
}

void CBoss_State_Attack_Magic::OnStateUpdate(_float fTimeDelta)
{
    Attack(fTimeDelta);
}

void CBoss_State_Attack_Magic::OnStateExit()
{
    m_pBoss->Reset_AttackTime();
    m_pBoss->Change_Target();
}


void CBoss_State_Attack_Magic::Attack(_float fTimeDelta)
{
    if (!m_isAttack && m_pBoss->Get_CurrentTrackPosition() >= 47.f) {
        m_isAttack = true;
        m_pBoss->Create_Electricity();
        m_pGameInstance->PlaySoundDuplicate(TEXT("Boss_Attack_Magic.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
    }

    if (m_pBoss->Is_Animation_Finished())
        m_pBoss->Change_State(CBoss::STATE_IDLE);
}

CBoss_State_Attack_Magic* CBoss_State_Attack_Magic::Create(CBoss* pBoss)
{
    CBoss_State_Attack_Magic* pInstance = new CBoss_State_Attack_Magic(pBoss);
    return pInstance;
}

void CBoss_State_Attack_Magic::Free()
{
    __super::Free();
}
