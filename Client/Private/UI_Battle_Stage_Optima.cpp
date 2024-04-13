#include "stdafx.h"
#include "UI_Battle_Stage_Optima.h"
#include "Player_Battle.h"
#include "UI_Pnal.h"
#include "UI_Optima_Info.h"

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

	for (auto& pOptima_Info : m_Optima_Infos) {
		pOptima_Info->Set_Render(true);
		pOptima_Info->Reset_Position();
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

	for (auto& pOptima_Info : m_Optima_Infos) {
		pOptima_Info->Set_Render(false);
	}
}

void CUI_Battle_Stage_Optima::Start()
{
	Add_Pnals();
	Add_Optima_Infos();
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

HRESULT CUI_Battle_Stage_Optima::Add_Optima_Infos()
{
	CUI_Optima_Info* pOptima_Info = { nullptr };

	CUI_Optima_Info::UI_OPTIMA_INFO_DESC UI_Optima_Info_Desc = {};
	UI_Optima_Info_Desc.vStartPosition = { (_float)g_iWinSizeX * 0.5f - 45.f , -140.f, 0.f };
	UI_Optima_Info_Desc.vTargetPosition = { -45.f, -140.f , 0.f };

	for (_int i = 0; i < m_pPlayer_Battle->Get_Optima_Size(); ++i) {
		UI_Optima_Info_Desc.iChrNum = i;
		UI_Optima_Info_Desc.vStartPosition.x += 45.f;
		UI_Optima_Info_Desc.vTargetPosition.x += 45.f;
		UI_Optima_Info_Desc.vStartPosition.y -= 45.f;
		UI_Optima_Info_Desc.vTargetPosition.y -= 45.f;

		pOptima_Info = dynamic_cast<CUI_Optima_Info*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Optima_Info"), &UI_Optima_Info_Desc));
		m_Optima_Infos.push_back(pOptima_Info);
	}

	for (auto& iter : m_Optima_Infos) {
		iter->Set_Render(false);
		Safe_AddRef(iter);
	}

	Update_Optima_Infos();
	
	return S_OK;
}

void CUI_Battle_Stage_Optima::Update_Cursor()
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_DOWN)) {
		if (m_iCursor + 1 < m_Pnals.size()) {
			++m_iCursor;
			Update_Optima_Infos();
		}
	}

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_UP)) {
		if (m_iCursor > 0) {
			--m_iCursor;
			Update_Optima_Infos();
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

void CUI_Battle_Stage_Optima::Update_Optima_Infos()
{
	COptima::Optima* pOptima = m_pPlayer_Battle->Get_Optima(m_iCursor);
	
	for (int i = 0; i < 3; ++i) {
		m_Optima_Infos[i]->Set_Optima((*pOptima)[i]);
	}

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

	for (auto& pOptima_Info : m_Optima_Infos)
		Safe_Release(pOptima_Info);
	m_Optima_Infos.clear();

}
