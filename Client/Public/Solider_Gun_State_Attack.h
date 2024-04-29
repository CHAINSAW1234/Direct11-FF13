#pragma once
#include "FSM_State.h"

BEGIN(Client)
class CSolider_Gun_State_Attack final : public CFSM_State
{
private:
	CSolider_Gun_State_Attack(class CSolider_Gun* pSolider_Gun);
	virtual ~CSolider_Gun_State_Attack() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	void Attack(_float fTimeDelta);

private:
	class CSolider_Gun* m_pSolider_Gun = { nullptr };
	_bool m_isAttack[3] = { false };

public:
	static	CSolider_Gun_State_Attack* Create(class CSolider_Gun* pSolider_Gun);
	virtual void Free() override;

};

END