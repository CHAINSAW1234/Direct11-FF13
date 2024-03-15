#include "stdafx.h"
#include "Chr_Battle_Light_Attack.h"

CChr_Battle_Light_Attack::CChr_Battle_Light_Attack(CChr_Battle_Light* pChr_Battle_Light)
{
}

void CChr_Battle_Light_Attack::OnStateEnter()
{
}

void CChr_Battle_Light_Attack::OnStateUpdate(_float fTimeDelta)
{
}

void CChr_Battle_Light_Attack::OnStateExit()
{
}

CChr_Battle_Light_Attack* CChr_Battle_Light_Attack::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Attack* pInstance = new CChr_Battle_Light_Attack(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_Attack::Free()
{
	__super::Free();
}
