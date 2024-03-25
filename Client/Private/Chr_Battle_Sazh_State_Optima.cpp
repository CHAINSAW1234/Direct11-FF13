#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Optima.h"
#include "Chr_Battle_Sazh.h"

CChr_Battle_Sazh_State_Optima::CChr_Battle_Sazh_State_Optima(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
    m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Optima::OnStateEnter()
{
    m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::OPTIMACHANGE_FIRST, false);
}

void CChr_Battle_Sazh_State_Optima::OnStateUpdate(_float fTimeDelta)
{
    if (m_pChr_Battle_Sazh->Is_Animation_Finished()) {
        m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::IDLE);
    }
}

void CChr_Battle_Sazh_State_Optima::OnStateExit()
{
}

CChr_Battle_Sazh_State_Optima* CChr_Battle_Sazh_State_Optima::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
    CChr_Battle_Sazh_State_Optima* pInstance = new CChr_Battle_Sazh_State_Optima(pChr_Battle_Sazh);
    return pInstance;
}

void CChr_Battle_Sazh_State_Optima::Free()
{
    __super::Free();
}
