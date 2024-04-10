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
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

	virtual void Start() override;

private:
	HRESULT Add_UI();

private:
	CPlayer_Battle* m_pPlayer_Battle = { nullptr };
	CUI_Optima_Change* m_pUI_Optima_Change = { nullptr };
	// UI �ϳ� �ʿ���

public:
	static	CUI_Battle_Stage_Wait_Optima* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END