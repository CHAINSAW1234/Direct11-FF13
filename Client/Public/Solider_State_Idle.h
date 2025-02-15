#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CSolider_State_Idle final : public CFSM_State
{
	enum STATE { IDLE, MOVE_BACK, MOVE_LEFT, MOVE_RIGHT, MOVE, MOVE_BACK_JUMP, TURN, STATE_END };
private:
	CSolider_State_Idle(class CSolider* pSolider);
	virtual ~CSolider_State_Idle() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	void Idle(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Move_Left(_float fTimeDelta);
	void Move_Right(_float fTimeDelta);
	void Move_Back(_float fTimeDelta);
	void Move_Back_Jump(_float fTimeDelta);
	void Turn(_float fTimeDelta);
	void Change_State(STATE eState);

private:
	class CSolider* m_pSolider = { nullptr };
	STATE			m_eState = { STATE_END };
	_float			m_fDegree = { 0.f };
	_float			m_fTimeDelta = { 0.f };
	_float			m_fStateTime = { 0.f };
	_float			m_fPrevTimeDelta = { 0.f };
	_bool			m_isMoveTurn = { false };
public:
	static	CSolider_State_Idle* Create(class CSolider* pSolider);
	virtual void Free() override;

};

END