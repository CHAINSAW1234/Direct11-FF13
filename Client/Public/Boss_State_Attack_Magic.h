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
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
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