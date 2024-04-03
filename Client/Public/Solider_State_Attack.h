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
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
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