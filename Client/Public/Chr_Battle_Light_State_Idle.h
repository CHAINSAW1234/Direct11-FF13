#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_State_Idle final : public CFSM_State
{
	enum STATE { IDLE, HURT, MOVE, ATTACK_READY, TURN, STATE_END };					// TURN ���߿� ����
	enum MOVEMENT { STRAIGHT, BACKWARD, NEXT, TO_START, MOVEMENT_END} ;
private:
	CChr_Battle_Light_State_Idle(class CChr_Battle_Light* pChr_Battle_Light);
	virtual ~CChr_Battle_Light_State_Idle() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Idle(_float fTimeDelta);
	void Hurt();
	void Move(_float fTimeDelta);

	void Change_MovementAnimation();
	void Update_Movement();
	void Update_LookAt();			// Target ��ġ�� �� Look���Ͱ��� ������ ����ؼ� ������
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
	_float		m_fTurnCheckTimeDelta = { 0.f };		// ���� üũ�� �ð� ��� ����
	_float		m_fMoveTimeDelta = { 0.f };			// �̵� �ð��� �ʹ� ��� �ȵ� 

	_bool		m_isPatternEnable = { true };		// �̵� ���� ���� ���� -> ����, �ǰݽ� �ʱ�ȭ	-> �ǰݽÿ��� ��, �� �̵� ���� �ε�
												// false�� ��� Startposition��ġ���� �ֵ��� �ּ����� Move
public:
	static	CChr_Battle_Light_State_Idle* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END
