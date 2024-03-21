#include "stdafx.h"
#include "UI_Battle_Stage_Item.h"
#include "Player_Battle.h"
#include "Inventory.h"
#include "UI_Pnal.h"

CUI_Battle_Stage_Item::CUI_Battle_Stage_Item(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Item::OnStateEnter()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(true);
		pPnal->Reset_Position();
	}
	m_pPlayer_Battle->Set_CursorPosition(m_Pnals[m_iCursor]->Get_TargetPosition());
}

void CUI_Battle_Stage_Item::OnStateUpdate(_float fTimeDelta)
{
	Update_Cursor();
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_BACKSPACE)) {
		m_pPlayer_Battle->Back_Stage();
	}

}

void CUI_Battle_Stage_Item::OnStateExit()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(false);
	}
}

void CUI_Battle_Stage_Item::Start()
{
	Add_Pnals();
}

HRESULT CUI_Battle_Stage_Item::Add_Pnals()
{
	CUI_Pnal* pPnal = { nullptr };

	CUI_Pnal::UI_PNAL_DESC UI_Pnal_Desc = {};
	UI_Pnal_Desc.vStartPosition = { (_float)g_iWinSizeX * -0.5f + 15.f, -200.f + 25, 0.f };
	UI_Pnal_Desc.vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 200 + 15.f,-200.f + 25, 0.f };
	UI_Pnal_Desc.vTargetPosition.x += 128 * 1.3f + 5;
	UI_Pnal_Desc.vStartPosition.x += 128 * 1.3f + 5;

	CInventory* pInventory = m_pPlayer_Battle->Get_Inventory();

	for (size_t i = 0; i < pInventory->Get_Inventory_Count(); ++i) {
		if (i % 2) {
			UI_Pnal_Desc.vTargetPosition.x += 128 * 1.3f + 5;
			UI_Pnal_Desc.vStartPosition.x += 128 * 1.3f + 5;
		}
		else {
			UI_Pnal_Desc.vTargetPosition.x -= 128 * 1.3f + 5;
			UI_Pnal_Desc.vStartPosition.x -= 128 * 1.3f + 5;
			UI_Pnal_Desc.vStartPosition.y -= 25.f;
			UI_Pnal_Desc.vTargetPosition.y -= 25.f;
		}

		CInventory::ITEM eItem = pInventory->Get_Item_Index(i);
		UI_Pnal_Desc.strName = pInventory->Get_Item_Name(eItem);
		pPnal = dynamic_cast<CUI_Pnal*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Pnal"), TEXT("Prototype_GameObject_UI_Pnal"), &UI_Pnal_Desc));
		m_Pnals.push_back(pPnal);
	}


	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Color(vItemColor);
		Safe_AddRef(pPnal);
		pPnal->Set_Render(false);
	}

	return S_OK;
}

void CUI_Battle_Stage_Item::Update_Cursor()
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_DOWN)) {
		if (m_iCursor + 2 < m_Pnals.size()) {
			m_iCursor += 2;
		}
	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_UP)) {
		if (m_iCursor - 2 >= 0) {
			m_iCursor -= 2;
		}
	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RIGHT)) {
		if (!(m_iCursor%2) && m_iCursor < m_Pnals.size() - 1) {
			++m_iCursor;
		}
	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_LEFT)) {
		if ((m_iCursor % 2) && m_iCursor > 0) {
			--m_iCursor;
		}
	}

	m_pPlayer_Battle->Set_CursorPosition(m_Pnals[m_iCursor]->Get_TargetPosition());

}

CUI_Battle_Stage_Item* CUI_Battle_Stage_Item::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Item* pInstance = new CUI_Battle_Stage_Item(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Item::Free()
{
	__super::Free();
	for (auto& pPnal : m_Pnals)
		Safe_Release(pPnal);

	m_Pnals.clear();
}
