#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CLeopard_State_Start final : public CFSM_State
{
private:
	CLeopard_State_Start(class CLeopard* pLeopard);
	virtual ~CLeopard_State_Start() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	class CLeopard* m_pLeopard = { nullptr };

public:
	static	CLeopard_State_Start* Create(class CLeopard* pLeopard);
	virtual void Free() override;

};

END