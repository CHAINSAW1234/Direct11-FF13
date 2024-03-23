#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Finish.h"
#include "Chr_Battle_Sazh.h"

CChr_Battle_Sazh_State_Finish::CChr_Battle_Sazh_State_Finish(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Finish::OnStateEnter()
{
	m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::ANIM_FINISH, false);
}

void CChr_Battle_Sazh_State_Finish::OnStateUpdate(_float fTimeDelta)
{

}

void CChr_Battle_Sazh_State_Finish::OnStateExit()
{
}

CChr_Battle_Sazh_State_Finish* CChr_Battle_Sazh_State_Finish::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	CChr_Battle_Sazh_State_Finish* pInstance = new CChr_Battle_Sazh_State_Finish(pChr_Battle_Sazh);

	return pInstance;
}

void CChr_Battle_Sazh_State_Finish::Free()
{
	__super::Free();
}
