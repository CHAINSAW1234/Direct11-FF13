#include "stdafx.h"
#include "UI_Battle_Stage_Target_Member.h"
#include "Player_Battle.h"
#include "UI_Pnal.h"

CUI_Battle_Stage_Target_Member::CUI_Battle_Stage_Target_Member(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Target_Member::OnStateEnter()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(true);
		pPnal->Reset_Position();
	}
}

void CUI_Battle_Stage_Target_Member::OnStateUpdate(_float fTimeDelta)
{
	Update_Cursor();

	Update_KeyInput();
}

void CUI_Battle_Stage_Target_Member::OnStateExit()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(false);
	}
}

void CUI_Battle_Stage_Target_Member::Start()
{
	Add_Pnals();
}

HRESULT CUI_Battle_Stage_Target_Member::Add_Pnals()
{
	CUI_Pnal* pPnal = { nullptr };
	CUI_Pnal::UI_PNAL_DESC UI_Pnal_Desc = {};
	UI_Pnal_Desc.vStartPosition = { (_float)g_iWinSizeX * -0.5f - 15.f,-200.f + 25.f,0.f };
	UI_Pnal_Desc.vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 215.f, -200.f + 25.f,0.f };

	// 1. Leader 

	UI_Pnal_Desc.vStartPosition.y -= 25.f;
	UI_Pnal_Desc.vTargetPosition.y -= 25.f;
	//UI_Pnal_Desc.strName = pMonster->Get_Name();
	pPnal = dynamic_cast<CUI_Pnal*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Pnal"), TEXT("Prototype_GameObject_UI_Pnal"), &UI_Pnal_Desc));
	m_Pnals.push_back(pPnal);


	// 2. Memeber
	vector<CChr_Battle*> vecMembers = m_pPlayer_Battle->Get_Members();

	for (auto& pMemeber : vecMembers) {
		UI_Pnal_Desc.vStartPosition.y -= 25.f;
		UI_Pnal_Desc.vTargetPosition.y -= 25.f;
		//UI_Pnal_Desc.strName = pMonster->Get_Name();
		pPnal = dynamic_cast<CUI_Pnal*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Pnal"), TEXT("Prototype_GameObject_UI_Pnal"), &UI_Pnal_Desc));
		m_Pnals.push_back(pPnal);
	}

	for (auto& iter : m_Pnals) {
		Safe_AddRef(iter);
		iter->Set_Render(false);
	}

	return S_OK;

}

void CUI_Battle_Stage_Target_Member::Update_Cursor()
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

void CUI_Battle_Stage_Target_Member::Update_KeyInput()
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_BACKSPACE)) {
		m_pPlayer_Battle->Cancel_Item();
		m_pPlayer_Battle->Back_Stage();

	}

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RETURN)) {
		m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_WAIT);

	}

}

CUI_Battle_Stage_Target_Member* CUI_Battle_Stage_Target_Member::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Target_Member* pInstance = new CUI_Battle_Stage_Target_Member(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Target_Member::Free()
{
	__super::Free();

	for (auto& pPnal : m_Pnals)
		Safe_Release(pPnal);
	m_Pnals.clear();
}
