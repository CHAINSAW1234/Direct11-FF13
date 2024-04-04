#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CBoss_State_Hit final : public CFSM_State
{
private:
	CBoss_State_Hit(class CBoss* pBoss);
	virtual ~CBoss_State_Hit() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	class CBoss* m_pBoss = { nullptr };

public:
	static	CBoss_State_Hit* Create(class CBoss* pBoss);
	virtual void Free() override;

};

END