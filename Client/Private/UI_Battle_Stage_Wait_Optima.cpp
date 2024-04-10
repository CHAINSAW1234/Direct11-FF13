#include "stdafx.h"
#include "UI_Battle_Stage_Wait_Optima.h"
#include "Player_Battle.h"
#include "UI_Optima_Change.h"
#include "Optima.h"
CUI_Battle_Stage_Wait_Optima::CUI_Battle_Stage_Wait_Optima(CPlayer_Battle* pPlayer_Battle)
{
	m_pPlayer_Battle = pPlayer_Battle;
}

void CUI_Battle_Stage_Wait_Optima::OnStateEnter()
{
	m_pUI_Optima_Change->Set_OptimaName(m_pPlayer_Battle->Get_Current_Optima_Name());
	m_pUI_Optima_Change->Reset_Position();
}

void CUI_Battle_Stage_Wait_Optima::OnStateUpdate(_float fTimeDelta)
{
	if (m_pUI_Optima_Change->isFinish()) {
		m_pPlayer_Battle->Change_Stage(CPlayer_Battle::STAGE_SELECT);
	}
}

void CUI_Battle_Stage_Wait_Optima::OnStateExit()
{
	m_pUI_Optima_Change->Set_Render(false);
}

void CUI_Battle_Stage_Wait_Optima::Start()
{
	Add_UI();
}

HRESULT CUI_Battle_Stage_Wait_Optima::Add_UI()
{
	m_pUI_Optima_Change = (CUI_Optima_Change*)m_pGameInstance->Add_Clone_With_Object(g_Level, TEXT("Layer_UI"), TEXT("Prototype_GameObject_UI_Optima_Change"));
	Safe_AddRef(m_pUI_Optima_Change);

	return S_OK;
}

CUI_Battle_Stage_Wait_Optima* CUI_Battle_Stage_Wait_Optima::Create(CPlayer_Battle* pPlayer_Battle)
{
	CUI_Battle_Stage_Wait_Optima* pInstance = new CUI_Battle_Stage_Wait_Optima(pPlayer_Battle);

	return pInstance;
}

void CUI_Battle_Stage_Wait_Optima::Free()
{
	__super::Free();
	Safe_Release(m_pUI_Optima_Change);
}
