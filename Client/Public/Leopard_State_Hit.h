#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CLeopard_State_Hit final : public CFSM_State
{
private:
	CLeopard_State_Hit(class CLeopard* m_pLeopard);
	virtual ~CLeopard_State_Hit() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;


private:
	class CLeopard* m_pLeopard = { nullptr };

public:
	static	CLeopard_State_Hit* Create(class CLeopard* m_pLeopard);
	virtual void Free() override;

};

END