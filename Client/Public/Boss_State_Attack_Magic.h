#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CBoss_State_Attack_Magic final : public CFSM_State
{
private:
	CBoss_State_Attack_Magic(class CBoss* pBoss);
	virtual ~CBoss_State_Attack_Magic() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Attack(_float fTimeDelta);
	void Create_Electricity();

private:
	class CBoss* m_pBoss = { nullptr };
	_bool m_isAttack = { false };

public:
	static	CBoss_State_Attack_Magic* Create(class CBoss* pBoss);
	virtual void Free() override;

};

END