#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CPlayer_Battle;
class CUI_Battle_Stage_Begin final : public CFSM_State
{
private:
	CUI_Battle_Stage_Begin(CPlayer_Battle* pPlayer_Battle);
	virtual ~CUI_Battle_Stage_Begin() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;


private:
	CPlayer_Battle* m_pPlayer_Battle = { nullptr };
	_float m_fTimeDelta = { 0.f };
public:
	static	CUI_Battle_Stage_Begin* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;
};

END