#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CWarload_State_Skill final : public CFSM_State
{
	enum STATE { CHARGE, HEAL, STATE_END };
private:
	CWarload_State_Skill(class CWarload* pWarload);
	virtual ~CWarload_State_Skill() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Change_State(STATE eState);
	void Charge(_float fTimeDelta);
	void Heal(_float fTimeDelta);

private:
	class CWarload* m_pWarload = { nullptr };
	class CUI_Skill* m_pUI_Skill = { nullptr };
	STATE			m_eState = { STATE_END };
	_float			m_fTimeDelta = { 0.f };
	_int			m_iCount = { 0 };

public:
	static	CWarload_State_Skill* Create(class CWarload* pWarload);
	virtual void Free() override;

};

END