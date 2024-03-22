#include "stdafx.h"
#include "Chr_Battle_Light_Item.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_Item::CChr_Battle_Light_Item(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_Item::OnStateEnter()
{
	m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ITEM_SELF, false);
}

void CChr_Battle_Light_Item::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Use_Item();
		m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
	}

}

void CChr_Battle_Light_Item::OnStateExit()
{
}

CChr_Battle_Light_Item* CChr_Battle_Light_Item::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Item* pInstance = new CChr_Battle_Light_Item(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_Item::Free()
{
	__super::Free();
}
