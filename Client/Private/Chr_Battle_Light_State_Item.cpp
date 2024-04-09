#include "stdafx.h"
#include "Chr_Battle_Light_State_Item.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_State_Item::CChr_Battle_Light_State_Item(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_State_Item::OnStateEnter()
{
	if(!XMVector3Equal(m_pChr_Battle_Light->Get_Transform()->Get_State_Vector(CTransform::STATE_POSITION), XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position())))
		m_pChr_Battle_Light->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Light->Get_Target_Position()));
	m_pChr_Battle_Light->Change_Animation(CChr_Battle_Light::ITEM_SELF, false);
}

void CChr_Battle_Light_State_Item::OnStateUpdate(_float fTimeDelta)
{
	if (m_pChr_Battle_Light->Is_Animation_Finished()) {
		m_pChr_Battle_Light->Use_Item();
		m_pChr_Battle_Light->Change_State(CChr_Battle_Light::IDLE);
	}

}

void CChr_Battle_Light_State_Item::OnStateExit()
{
	m_pChr_Battle_Light->Set_Target(m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, 0));
}

CChr_Battle_Light_State_Item* CChr_Battle_Light_State_Item::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_State_Item* pInstance = new CChr_Battle_Light_State_Item(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_State_Item::Free()
{
	__super::Free();
}
