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
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
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