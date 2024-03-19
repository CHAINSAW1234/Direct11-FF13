#include "stdafx.h"
#include "UI_Battle_Stage_Command.h"

CUI_Battle_Stage_Command::CUI_Battle_Stage_Command(CPlayer_Battle* pPlayer_Battle)
{
}

void CUI_Battle_Stage_Command::OnStateEnter()
{
}

void CUI_Battle_Stage_Command::OnStateUpdate(_float fTimeDelta)
{
}

void CUI_Battle_Stage_Command::OnStateExit()
{
}

void CUI_Battle_Stage_Command::Render()
{
}

HRESULT CUI_Battle_Stage_Command::Initialize()
{
	return E_NOTIMPL;
}

CUI_Battle_Stage_Command* CUI_Battle_Stage_Command::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Command* pInstance = new CUI_Battle_Stage_Command(pPlayer_Battle);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed To Created : CUI_Battle_Stage_Command"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CUI_Battle_Stage_Command::Free()
{
	__super::Free();
}
