#include "stdafx.h"
#include "UI_Battle_Stage_Optima.h"
#include "Player_Battle.h"
#include "UI_Pnal.h"

CUI_Battle_Stage_Optima::CUI_Battle_Stage_Optima(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Optima::OnStateEnter()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(true);
		pPnal->Reset_Position();
	}
	m_pPlayer_Battle->Set_Command_Render(false);

	m_iCursor = m_pPlayer_Battle->Get_Current_Optima_Num();
	m_pPlayer_Battle->Set_CursorPosition(m_Pnals[m_iCursor]->Get_TargetPosition());
}

void CUI_Battle_Stage_Optima::OnStateUpdate(_float fTimeDelta)
{
	Update_Cursor();

	Update_KeyInput();
}

void CUI_Battle_Stage_Optima::OnStateExit()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(false);
	}
	m_pPlayer_Battle->Set_Command_Render(true);
}

void CUI_Battle_Stage_Optima::Start()
{
	Add_Pnals();
}

HRESULT CUI_Battle_Stage_Optima::Add_Pnals()
{
	CUI_Pnal* pPnal = { nullptr };

	CUI_Pnal::UI_PNAL_DESC UI_Pnal_Desc = {};
	UI_Pnal_Desc.vStartPosition = { (_float)g_iWinSizeX * -0.5f + 15.f, -150.f + 33.f, 0.f };
	UI_Pnal_Desc.vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 200 + 15.f, -150.f + 33.f, 0.f };
	UI_Pnal_Desc.strName = TEXT("");

	for (int i = 0; i < m_pPlayer_Battle->Get_Optima_Size(); ++i) {
		UI_Pnal_Desc.vStartPosition.y -= 33.f;
		UI_Pnal_Desc.vTargetPosition.y -= 33.f;
		UI_Pnal_Desc.strName = m_pPlayer_Battle->Get_Optima_Name(i);

		pPnal = dynamic_cast<CUI_Pnal*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Pnal"), TEXT("Prototype_GameObject_UI_Pnal"), &UI_Pnal_Desc));
		m_Pnals.push_back(pPnal);

	}

	for (auto& iter : m_Pnals) {
		iter->Set_Render(false);
		Safe_AddRef(iter);

	}

	return S_OK;
}

void CUI_Battle_Stage_Optima::Update_Cursor()
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

void CUI_Battle_Stage_Optima::Update_KeyInput()
{

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_BACKSPACE)) {
		m_pPlayer_Battle->Back_Stage();
	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_TAB)) {
		m_pPlayer_Battle->Back_Stage();
	}

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RETURN)) {
		if (m_iCursor != m_pPlayer_Battle->Get_Current_Optima_Num()) {
			m_pPlayer_Battle->Change_Optima(m_iCursor);
			m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_WAIT_OPTIMA);
		}

	}
}

void CUI_Battle_Stage_Optima::Update_Pnals()
{
}

CUI_Battle_Stage_Optima* CUI_Battle_Stage_Optima::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Optima* pInstance = new CUI_Battle_Stage_Optima(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Optima::Free()
{
	__super::Free();

	for (auto& pPnal : m_Pnals)
		Safe_Release(pPnal);
	m_Pnals.clear();

}
