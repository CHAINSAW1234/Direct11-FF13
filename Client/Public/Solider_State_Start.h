#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CSolider_State_Start final : public CFSM_State
{
private:
	CSolider_State_Start(class CSolider* pSolider);
	virtual ~CSolider_State_Start() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	class CSolider* m_pSolider = { nullptr };

public:
	static	CSolider_State_Start* Create(class CSolider* pSolider);
	virtual void Free() override;

};

END