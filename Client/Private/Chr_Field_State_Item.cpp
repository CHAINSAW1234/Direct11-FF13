#include "stdafx.h"
#include "Chr_Field_State_Item.h"
#include "Chr_Field.h"

CChr_Field_State_Item::CChr_Field_State_Item(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Item::OnStateEnter()
{
	m_pChr_Field->Change_Animation(CChr_Field::ITEM_START, false);
}

void CChr_Field_State_Item::OnStateUpdate(_float fTimeDelta)
{
	Update_Animation();
}

void CChr_Field_State_Item::OnStateExit()
{
	m_pChr_Field->Change_Animation(CChr_Field::ITEM_END, false);
}

void CChr_Field_State_Item::Update_Animation()
{
	if (!m_pChr_Field->Is_Animation_Finished())
		return;
	
	if (m_pChr_Field->Get_CurrentAnimationIndex() == CChr_Field::ITEM_START)
		m_pChr_Field->Change_Animation(CChr_Field::ITEM_IDLE, true);
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
