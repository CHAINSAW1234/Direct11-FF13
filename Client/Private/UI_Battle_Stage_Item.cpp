#include "stdafx.h"
#include "UI_Battle_Stage_Item.h"

CUI_Battle_Stage_Item::CUI_Battle_Stage_Item(CPlayer_Battle* pPlayer_Battle)
{
}

void CUI_Battle_Stage_Item::OnStateEnter()
{
}

void CUI_Battle_Stage_Item::OnStateUpdate(_float fTimeDelta)
{
}

void CUI_Battle_Stage_Item::OnStateExit()
{
}

void CUI_Battle_Stage_Item::Render()
{
}

HRESULT CUI_Battle_Stage_Item::Initialize()
{
	return E_NOTIMPL;
}

CUI_Battle_Stage_Item* CUI_Battle_Stage_Item::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Item* pInstance = new CUI_Battle_Stage_Item(pPlayer_Battle);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Battle_Stage_Item"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Battle_Stage_Item::Free()
{
	__super::Free();
}
