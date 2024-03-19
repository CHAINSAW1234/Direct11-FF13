#include "stdafx.h"
#include "UI_Battle_Stage_Target.h"

CUI_Battle_Stage_Target::CUI_Battle_Stage_Target(CPlayer_Battle* pPlayer_Battle)
{
}

void CUI_Battle_Stage_Target::OnStateEnter()
{
}

void CUI_Battle_Stage_Target::OnStateUpdate(_float fTimeDelta)
{
}

void CUI_Battle_Stage_Target::OnStateExit()
{
}

void CUI_Battle_Stage_Target::Render()
{
}

HRESULT CUI_Battle_Stage_Target::Initialize()
{
	return E_NOTIMPL;
}

CUI_Battle_Stage_Target* CUI_Battle_Stage_Target::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Target* pInstance = new CUI_Battle_Stage_Target(pPlayer_Battle);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Battle_Stage_Target"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Battle_Stage_Target::Free()
{
	__super::Free();
}
