#include "stdafx.h"
#include "Boss_State_Attack_Magic.h"
#include "Boss.h"
#include "Electricity.h"

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
        Create_Electricity();
    }

    if (m_pBoss->Is_Animation_Finished())
        m_pBoss->Change_State(CBoss::STATE_IDLE);
}

void CBoss_State_Attack_Magic::Create_Electricity()
{
    CElectricity::ELECTRICITY_DESC pDesc;
    pDesc.fStartPosition = m_pBoss->Get_BonePos("chin_a");
    pDesc.fTargetPosition = m_pBoss->Get_TargetPosition();
    pDesc.TurnDist = 0;
    if(FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity"), &pDesc)))
        return ;

    pDesc.fTargetPosition.x += Random_Float(1.f);
    pDesc.fTargetPosition.z += Random_Float(1.f);
    pDesc.TurnDist = 60;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity"), &pDesc)))
        return;

    pDesc.fTargetPosition.x += Random_Float(1.f);
    pDesc.fTargetPosition.z += Random_Float(1.f);
    pDesc.TurnDist = 120;
    if (FAILED(m_pGameInstance->Add_Clone(g_Level, TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Electricity"), &pDesc)))
        return;

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
