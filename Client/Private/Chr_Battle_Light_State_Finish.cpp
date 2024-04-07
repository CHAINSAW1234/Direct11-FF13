#include "stdafx.h"
#include "Chr_Battle_Light_State_Finish.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_State_Finish::CChr_Battle_Light_State_Finish(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_State_Finish::OnStateEnter()
{
	m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ANIM_FINISH, false);
	m_pChr_Battle_Light->Change_Animation_Weapon(CChr_Battle_Light::WEAPON_CLOSE);

}

void CChr_Battle_Light_State_Finish::OnStateUpdate(_float fTimeDelta)
{
}

void CChr_Battle_Light_State_Finish::OnStateExit()
{
	m_pChr_Battle_Light->Change_Animation_Weapon(CChr_Battle_Light::WEAPON_OPEN_IDLE);
}

CChr_Battle_Light_State_Finish* CChr_Battle_Light_State_Finish::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_State_Finish* pInstance = new CChr_Battle_Light_State_Finish(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_State_Finish::Free()
{
	__super::Free();
}
