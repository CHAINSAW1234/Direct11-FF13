#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CBoss_State_Attack_Physic final : public CFSM_State
{
	enum STATE { MOVE, ATTACK, STATE_END };

private:
	CBoss_State_Attack_Physic(class CBoss* pBoss);
	virtual ~CBoss_State_Attack_Physic() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Move(_float fTimeDelta);
	void Attack(_float fTimeDelta);

private:
	class CBoss* m_pBoss = { nullptr };
	STATE m_eState = { STATE_END };
	_bool m_isAttack = { false };

public:
	static	CBoss_State_Attack_Physic* Create(class CBoss* pBoss);
	virtual void Free() override;

};

END