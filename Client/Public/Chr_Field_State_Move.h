#pragma once

#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CChr_Field_State_Move final : public CFSM_State
{
private:
	CChr_Field_State_Move(class CChr_Field* pChr_Field);
	virtual ~CChr_Field_State_Move() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	class CChr_Field* m_pChr_Field = { nullptr };

public:
	static	CChr_Field_State_Move* Create(class CChr_Field* pChr_Field);
	virtual void				  Free() override;

};

END