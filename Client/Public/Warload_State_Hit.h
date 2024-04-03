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
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	class CWarload* m_pWarload = { nullptr };

public:
	static	CWarload_State_Hit* Create(class CWarload* pWarload);
	virtual void Free() override;

};

END