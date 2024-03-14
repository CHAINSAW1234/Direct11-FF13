#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_Run final : public CFSM_State
{
private:
	CChr_Battle_Light_Run(class CChr_Battle_Light* pChr_Field);
	virtual ~CChr_Battle_Light_Run() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	void Update_Animation();

private:
	class CChr_Battle_Light* m_pChr_Battle_Light = { nullptr };

public:
	static	CChr_Battle_Light_Run* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END
