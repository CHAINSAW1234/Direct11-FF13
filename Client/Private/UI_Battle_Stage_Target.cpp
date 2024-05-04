#include "stdafx.h"
#include "UI_Battle_Stage_Target.h"
#include "Player_Battle.h"
#include "UI_Pnal.h"
#include "Monster.h"

CUI_Battle_Stage_Target::CUI_Battle_Stage_Target(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Target::OnStateEnter()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(true);
		pPnal->Reset_Position();
	}

	Update_Pnals();

	for (_int i = 0; i < (_int)m_pPlayer_Battle->Get_Monsters().size(); ++i) {
		m_pPlayer_Battle->Get_Monsters()[i]->Set_isTarget(false);
	}

	if (m_iCursor >= 0) {
		m_pPlayer_Battle->Get_Monsters()[m_iCursor]->Set_isTarget(true);
		m_pPlayer_Battle->Set_CursorPosition(m_Pnals[m_iCursor]->Get_TargetPosition());
	}

	m_pPlayer_Battle->Change_Chain_Target(m_pPlayer_Battle->Get_Monsters()[m_iCursor]);

}

void CUI_Battle_Stage_Target::OnStateUpdate(_float fTimeDelta)
{
	// 매 틱마다 패널의 이름 체크해서 변경 시키기

	Update_Pnals();

	Update_Cursor();

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_BACKSPACE)) {
		m_pPlayer_Battle->Back_Stage();
		m_pGameInstance->PlaySoundW(TEXT("System_Cancel.wav"), CSound_Manager::UI1, SOUND_DEFAULT);

	}

	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_RETURN)) {
		CGameObject* pTargetObject = m_pGameInstance->Get_GameObject(g_Level, g_strMonsterLayerTag, (_uint)m_iCursor);
		m_pPlayer_Battle->Set_Leader_Target(pTargetObject);
		m_pPlayer_Battle->Set_Member_Target(pTargetObject);
		m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_WAIT);
		m_pPlayer_Battle->Set_Chain_Move();
		m_pGameInstance->PlaySoundW(TEXT("System_Enter.wav"), CSound_Manager::UI1, SOUND_DEFAULT);
	}
}

void CUI_Battle_Stage_Target::OnStateExit()
{
	for (auto& pPnal : m_Pnals) {
		pPnal->Set_Render(false);
	}
}

void CUI_Battle_Stage_Target::Start()
{
	Add_Pnals();
}

HRESULT CUI_Battle_Stage_Target::Add_Pnals()
{
	CUI_Pnal* pPnal = { nullptr };
	CUI_Pnal::UI_PNAL_DESC UI_Pnal_Desc = {};
	UI_Pnal_Desc.vStartPosition = { (_float)g_iWinSizeX * -0.5f,-200.f + 33.f,0.f };
	UI_Pnal_Desc.vTargetPosition = { (_float)g_iWinSizeX * -0.5f + 200.f, -200.f + 33.f,0.f };


	vector<CMonster*> vecMonsters =  m_pPlayer_Battle->Get_Monsters();

	for (auto& pMonster : vecMonsters) {
		UI_Pnal_Desc.vTargetPosition.x += 15;
		UI_Pnal_Desc.vStartPosition.x += 15;
		UI_Pnal_Desc.vStartPosition.y -= 33.f;
		UI_Pnal_Desc.vTargetPosition.y -= 33.f;
		UI_Pnal_Desc.strName = pMonster->Get_Name();
		pPnal = dynamic_cast<CUI_Pnal*>(m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_Pnal"), TEXT("Prototype_GameObject_UI_Pnal"), &UI_Pnal_Desc));
		m_Pnals.push_back(pPnal);
	}

	for (auto& iter : m_Pnals) {
		Safe_AddRef(iter);
		iter->Set_Render(false);
	}

	return S_OK;
}

void CUI_Battle_Stage_Target::Update_Cursor()
{
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_DOWN)) {
		if (m_iCursor + 1 < m_Pnals.size()) {
			m_pPlayer_Battle->Get_Monsters()[m_iCursor]->Set_isTarget(false);
			++m_iCursor;
			m_pPlayer_Battle->Get_Monsters()[m_iCursor]->Set_isTarget(true);
			m_pPlayer_Battle->Change_Chain_Target(m_pPlayer_Battle->Get_Monsters()[m_iCursor]);
			m_pGameInstance->PlaySoundW(TEXT("System_Cursor.wav"), CSound_Manager::UI2, SOUND_DEFAULT);
		}

	}
	if (m_pGameInstance->Get_KeyState(KEY_DOWN, DIK_UP)) {
		if (m_iCursor > 0) {
			m_pPlayer_Battle->Get_Monsters()[m_iCursor]->Set_isTarget(false);
			--m_iCursor;
			m_pPlayer_Battle->Get_Monsters()[m_iCursor]->Set_isTarget(true);
			m_pPlayer_Battle->Change_Chain_Target(m_pPlayer_Battle->Get_Monsters()[m_iCursor]);
			m_pGameInstance->PlaySoundW(TEXT("System_Cursor.wav"), CSound_Manager::UI2, SOUND_DEFAULT);
		}

	}
	m_pPlayer_Battle->Set_CursorPosition(m_Pnals[m_iCursor]->Get_TargetPosition());

}

void CUI_Battle_Stage_Target::Update_Pnals()
{
	vector<CMonster*> vecMonsters = m_pPlayer_Battle->Get_Monsters();
	
	for (int i = 0; i < vecMonsters.size(); ++i) {
		m_Pnals[i]->Set_Name(vecMonsters[i]->Get_Name());
	}

	while (vecMonsters.size() < m_Pnals.size()) {
		m_Pnals.back()->Set_Dead(true);
		Safe_Release(m_Pnals.back());
		m_Pnals.pop_back();
	}

	if (m_iCursor > m_Pnals.size() - 1) {
		m_iCursor = m_Pnals.size() - 1;
		m_pPlayer_Battle->Get_Monsters()[m_iCursor]->Set_isTarget(true);
	}
}

CUI_Battle_Stage_Target* CUI_Battle_Stage_Target::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Target* pInstance = new CUI_Battle_Stage_Target(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Target::Free()
{
	__super::Free();

	for (auto& pPnal : m_Pnals)
		Safe_Release(pPnal);
	m_Pnals.clear();
}
