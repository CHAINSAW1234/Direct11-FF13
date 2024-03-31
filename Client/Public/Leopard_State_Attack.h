#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CLeopard_State_Attack final : public CFSM_State
{
	enum STATE { RUN, ATTACK, STATE_END};
private:
	CLeopard_State_Attack(class CLeopard* pLeopard);
	virtual ~CLeopard_State_Attack() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Run(_float fTimeDelta);
	void Attack();

private:
	class CLeopard* m_pLeopard = { nullptr };
	STATE			m_eState = { STATE_END };

public:
	static	CLeopard_State_Attack* Create(class CLeopard* pLeopard);
	virtual void Free() override;

};

END