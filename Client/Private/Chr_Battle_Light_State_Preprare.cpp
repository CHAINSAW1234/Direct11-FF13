#include "stdafx.h"
#include "Chr_Battle_Light_State_Preprare.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_Preprare::CChr_Battle_Light_Preprare(CChr_Battle_Light* pChr_Battle_Light)
{
    m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_Preprare::OnStateEnter()
{
    m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ATTACK_PREPARE, false);
}

void CChr_Battle_Light_Preprare::OnStateUpdate(_float fTimeDelta)
{
    m_pChr_Battle_Light->Update_ATB(fTimeDelta);

    if (m_pChr_Battle_Light->Is_Animation_Finished())
        m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
      
    m_pChr_Battle_Light->Determine_Action_Based_On_Command();
}

void CChr_Battle_Light_Preprare::OnStateExit()
{
}

CChr_Battle_Light_Preprare* CChr_Battle_Light_Preprare::Create(CChr_Battle_Light* pChr_Battle_Light)
{
    CChr_Battle_Light_Preprare* pInstance = new CChr_Battle_Light_Preprare(pChr_Battle_Light);

    return pInstance;
}

void CChr_Battle_Light_Preprare::Free()
{
    __super::Free();
}
