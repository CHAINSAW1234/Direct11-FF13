#include "stdafx.h"
#include "Boss_State_Attack_Physic.h"
#include "Boss.h"

CBoss_State_Attack_Physic::CBoss_State_Attack_Physic(CBoss* pBoss)
{
    m_pBoss = pBoss;
}

void CBoss_State_Attack_Physic::OnStateEnter()
{
    m_eState = MOVE;
    m_pBoss->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pBoss->Get_TargetPosition()));
    m_pBoss->Change_Animation(CBoss::MOVE_STRAIGHT_START, false);
    m_pBoss->Reset_Attakable();
}

void CBoss_State_Attack_Physic::OnStateUpdate(_float fTimeDelta)
{
    // 충돌 체크 함수 
    switch (m_eState) {
    case MOVE:
        Move(fTimeDelta);
        break;
    case ATTACK:
        Attack(fTimeDelta);
        break;
    }

}

void CBoss_State_Attack_Physic::OnStateExit()
{

    m_pBoss->Reset_AttackTime();
}

void CBoss_State_Attack_Physic::Move(_float fTimeDelta)
{
    _float fDist = m_pBoss->Cal_Dist_Target();

    m_pBoss->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pBoss->Get_TargetPosition()));

    if (fDist <= 1.f) {
        m_eState = ATTACK;
        m_pBoss->Change_Animation(CBoss::ATTACK1, false);
    }

    m_pBoss->Get_Transform()->Go_Straight(fTimeDelta * 3);

    if (m_pBoss->Is_Animation_Finished())
        m_pBoss->Change_Animation(CBoss::MOVE_STRAIGHT_IDLE, true);
}

void CBoss_State_Attack_Physic::Attack(_float fTimeDelta)
{
    if (20.f <= m_pBoss->Get_CurrentTrackPosition() &&
        m_pBoss->Get_CurrentTrackPosition() <= 30.f)
        m_pBoss->Check_Interact_Weapon();

    if (m_pBoss->Is_Animation_Finished())
        m_pBoss->Change_State(CBoss::STATE_IDLE);
}


CBoss_State_Attack_Physic* CBoss_State_Attack_Physic::Create(CBoss* pBoss)
{
    CBoss_State_Attack_Physic* pInstance = new CBoss_State_Attack_Physic(pBoss);
    return pInstance;
}

void CBoss_State_Attack_Physic::Free()
{
    __super::Free();
}
