#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Sazh_State_Idle final : public CFSM_State
{
	enum STATE { IDLE, HURT, MOVE, ATTACK_READY, TURN, STATE_END };
	enum MOVEMENT { BACKWARD, LEFT, RIGHT, STRAIGHT, MOVEMENT_END };
private:
	CChr_Battle_Sazh_State_Idle(class CChr_Battle_Sazh* pChr_Battle_Light);
	virtual ~CChr_Battle_Sazh_State_Idle() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	void Idle(_float fTimeDelta);
	void Hurt();
	void Move(_float fTimeDelta);
	void Turn(_float fTimeDelta);
	
	void Change_State(STATE eState);
	void Change_MovementAnimation();
	void Update_Hurt();
	_float4 Cal_Line();			// 시작점에서 타겟으로의 직선의 방정식을 구한다
	_float4 Cal_Dir_Start_To_Target();
	_float4 Cal_Current_Direction();	// 이동 방향을 계산해서 준다

private:
	class CChr_Battle_Sazh* m_pChr_Battle_Sazh = { nullptr };

	STATE		m_eState = { STATE_END };
	MOVEMENT	m_eMovement = { STRAIGHT };
	_bool		m_isHurt = { false };

	_float		m_fDegree = { 0.f };
	_float		m_fDist = { 5.f };
	_float		m_fTimeDelta = 0.f;
	_float		m_fStateTime = { 0.f };
	_float		m_fPrevTimeDelta = { 0.f };
	_bool		m_isMoveTurn = { false };

public:
	static	CChr_Battle_Sazh_State_Idle* Create(class CChr_Battle_Sazh* pChr_Battle_Light);
	virtual void Free() override;

};

END
