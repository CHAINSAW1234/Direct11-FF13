#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CWarload_State_Attack final : public CFSM_State
{
	enum STATE { RUN, ATTACK, STATE_END };
private:
	CWarload_State_Attack(class CWarload* pWarload);
	virtual ~CWarload_State_Attack() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Run(_float fTimeDelta);
	void Attack(_float fTimeDelta);

private:
	class CWarload* m_pWarload = { nullptr };
	STATE			m_eState = { STATE_END };

public:
	static	CWarload_State_Attack* Create(class CWarload* pWarload);
	virtual void Free() override;

};

END