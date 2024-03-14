#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CChr_Field_State_Walk final : public CFSM_State
{
	enum STATE { MOVE, TURN, STATE_END };
private:
	CChr_Field_State_Walk(class CChr_Field* pChr_Field);
	virtual ~CChr_Field_State_Walk() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	void	Move(_float fTimeDelta);
	void	Turn(_float fTimeDelta);

private:
	class CChr_Field* m_pChr_Field = { nullptr };
	STATE			  m_eState = { STATE_END };
	_float4			  m_vTargetDirection = { 0.f,0.f,0.f,0.f };
	_float			  m_fDegree = { 0.f };


public:
	static	CChr_Field_State_Walk* Create(class CChr_Field* pChr_Field);
	virtual void				  Free() override;

};

END
