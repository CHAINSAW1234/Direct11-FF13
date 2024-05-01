#include "stdafx.h"
#include "Chr_Battle_Light_State_Prepare.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_State_Prepare::CChr_Battle_Light_State_Prepare(CChr_Battle_Light* pChr_Battle_Light)
{
    m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_State_Prepare::OnStateEnter()
{
    m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_PREPARE, false);
}

void CChr_Battle_Light_State_Prepare::OnStateUpdate(_float fTimeDelta)
{
    m_pChr_Battle_Light->Update_ATB(fTimeDelta);
    m_pChr_Battle_Light->Determine_Action_Based_On_Command();

    if (m_pChr_Battle_Light->Is_Animation_Finished())
        m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
      
}

void CChr_Battle_Light_State_Prepare::OnStateExit()
{
}

CChr_Battle_Light_State_Prepare* CChr_Battle_Light_State_Prepare::Create(CChr_Battle_Light* pChr_Battle_Light)
{
    CChr_Battle_Light_State_Prepare* pInstance = new CChr_Battle_Light_State_Prepare(pChr_Battle_Light);

    return pInstance;
}

void CChr_Battle_Light_State_Prepare::Free()
{
    __super::Free();
}
