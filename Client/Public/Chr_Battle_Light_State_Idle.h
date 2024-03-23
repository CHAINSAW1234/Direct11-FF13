#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_State_Idle final : public CFSM_State
{
	enum STATE { IDLE, HURT, MOVE, ATTACK_READY, TURN, STATE_END };					// TURN 나중에 구현
	enum MOVEMENT { STRAIGHT, BACKWARD, NEXT, TO_START, MOVEMENT_END} ;
private:
	CChr_Battle_Light_State_Idle(class CChr_Battle_Light* pChr_Battle_Light);
	virtual ~CChr_Battle_Light_State_Idle() = default;

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

	void Change_MovementAnimation();
	void Update_Movement();
	void Update_LookAt();			// Target 위치와 내 Look벡터간의 각도를 계산해서 저장함
	_int Check_Movement_Next(FXMVECTOR vTargetPos, FXMVECTOR vLineDir, FXMVECTOR vCurPos);

private:
	class CChr_Battle_Light* m_pChr_Battle_Light = { nullptr };

	STATE		m_eState = { STATE_END };
	MOVEMENT	m_eMovement = { STRAIGHT };
	_bool		m_isHurt = { false };

	_float		m_fDegree = { 0.f };
	_float		m_fDist = { 5.f };
	_int		m_iNextDir = { 0 };

	_float		m_fTimeDelta = 0.f;
	_float		m_fTurnCheckTimeDelta = { 0.f };		// 방향 체크용 시간 계산 변수
	_float		m_fMoveTimeDelta = { 0.f };			// 이동 시간이 너무 길면 안됨 

	_bool		m_isPatternEnable = { true };		// 이동 패턴 실행 여부 -> 공격, 피격시 초기화	-> 피격시에는 앞, 뒤 이동 고정 인듯
												// false인 경우 Startposition위치에서 있도록 최소한의 Move
public:
	static	CChr_Battle_Light_State_Idle* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END
