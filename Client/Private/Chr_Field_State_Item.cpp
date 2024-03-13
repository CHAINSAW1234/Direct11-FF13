#include "stdafx.h"
#include "Chr_Field_State_Item.h"

CChr_Field_State_Item::CChr_Field_State_Item(CChr_Field* pChr_Field)
{
}

void CChr_Field_State_Item::OnStateEnter()
{
}

void CChr_Field_State_Item::OnStateUpdate(_float fTimeDelta)
{
}

void CChr_Field_State_Item::OnStateExit()
{
}

CChr_Field_State_Item* CChr_Field_State_Item::Create(CChr_Field* pChr_Field)
{
	CChr_Field_State_Item* pInstance = new CChr_Field_State_Item(pChr_Field);
	//m_pPlayer = pGameObject;

	return pInstance;
}

void CChr_Field_State_Item::Free()
{
	__super::Free();
}
