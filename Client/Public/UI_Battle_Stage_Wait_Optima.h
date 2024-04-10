#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CUI_Optima_Change;
class CPlayer_Battle;
class CUI_Battle_Stage_Wait_Optima final : public CFSM_State
{
private:
	CUI_Battle_Stage_Wait_Optima(CPlayer_Battle* pPlayer_Battle);
	virtual ~CUI_Battle_Stage_Wait_Optima() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

	virtual void Start() override;

private:
	HRESULT Add_UI();

private:
	CPlayer_Battle* m_pPlayer_Battle = { nullptr };
	CUI_Optima_Change* m_pUI_Optima_Change = { nullptr };
	// UI 하나 필요함

public:
	static	CUI_Battle_Stage_Wait_Optima* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END