#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CSolider_State_Attack final : public CFSM_State
{
	enum STATE { RUN, ATTACK, STATE_END };
private:
	CSolider_State_Attack(class CSolider* pSolider);
	virtual ~CSolider_State_Attack() = default;

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
	class CSolider* m_pSolider = { nullptr };
	STATE			m_eState = { STATE_END };


public:
	static	CSolider_State_Attack* Create(class CSolider* pSolider);
	virtual void Free() override;

};

END