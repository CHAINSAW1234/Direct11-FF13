#include "stdafx.h"
#include "UI_Battle_Stage_Select.h"
#include "Player_Battle.h"
#include "UI_Pnal.h"

CUI_Battle_Stage_Select::CUI_Battle_Stage_Select(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Select::OnStateEnter()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(true);
		pPnal->Reset_Position();
	}

	if (m_Pnals.size() > 0) {
		m_pPlayer_Battle->Set_CursorPosition(m_Pnals[m_iCursor]->Get_TargetPosition());
	}

}

void CUI_Battle_Stage_Select::OnStateUpdate(_float fTimeDelta)
{
	Update_Cursor();
	
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RETURN)) {
		Change_Stage();
	}
}

void CUI_Battle_Stage_Select::OnStateExit()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(false);
	}
}

void CUI_Battle_Stage_Select::Start()
{
	Add_Pnals();
}

HRESULT CUI_Battle_Stage_Select::Add_Pnals()
{
	CUI_Pnal* pPnal = { nullptr };
	CUI_Pnal::UI_PNAL_DESC UI_Pnal_Desc = {};
	UI_Pnal_Desc.vStartPosition = { (_float)g_iWinSizeX * -0.5f -15.f,-200.f + 25.f,0.f };
	UI_Pnal_Desc.vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 200,-200.f + 25.f,0.f };

	for (int i = 0; i < 3; ++i) {
		UI_Pnal_Desc.vStartPosition.y -= 25.f;
		UI_Pnal_Desc.vTargetPosition.x += 15.f;
		UI_Pnal_Desc.vTargetPosition.y -= 25.f;
		UI_Pnal_Desc.strName = strPnalName[i];
		pPnal = dynamic_cast<CUI_Pnal*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Pnal"), TEXT("Prototype_GameObject_UI_Pnal"), &UI_Pnal_Desc));
		m_Pnals.push_back(pPnal);
	}

	for (auto& iter : m_Pnals) {
		Safe_AddRef(iter);
	}

	return S_OK;
}

void CUI_Battle_Stage_Select::Update_Cursor()
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_DOWN)) {
		if (m_iCursor + 1 < m_Pnals.size()) {
			++m_iCursor;
		}
	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_UP)) {
		if (m_iCursor > 0) {
			--m_iCursor;
		}
	}

	m_pPlayer_Battle->Set_CursorPosition(m_Pnals[m_iCursor]->Get_TargetPosition());
}

void CUI_Battle_Stage_Select::Change_Stage()
{
	switch (m_iCursor) {
	case 0:
		//공격 큐 꽉 체워야 함
		m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_TARGET);
		break;
	case 1:
		m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_COMMAND);
		break;
	case 2:
		m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_ITEM);
		break;

	}

}

CUI_Battle_Stage_Select* CUI_Battle_Stage_Select::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Select* pInstance = new CUI_Battle_Stage_Select(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Select::Free()
{
	__super::Free();
	for (auto& pPnal : m_Pnals)
		Safe_Release(pPnal);

	m_Pnals.clear();
}
