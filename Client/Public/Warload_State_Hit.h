#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CWarload_State_Hit final : public CFSM_State
{
private:
	CWarload_State_Hit(class CWarload* pWarload);
	virtual ~CWarload_State_Hit() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	class CWarload* m_pWarload = { nullptr };

public:
	static	CWarload_State_Hit* Create(class CWarload* pWarload);
	virtual void Free() override;

};

END