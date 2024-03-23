#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Sazh_State_Idle final : public CFSM_State
{
	enum STATE { IDLE, HURT, MOVE, ATTACK_READY, TURN, STATE_END };
	enum MOVEMENT { STRAIGHT, BACKWARD, NEXT, TO_START, MOVEMENT_END };
private:
	CChr_Battle_Sazh_State_Idle(class CChr_Battle_Sazh* pChr_Battle_Light);
	virtual ~CChr_Battle_Sazh_State_Idle() = default;

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
	void Turn(_float fTimeDelta);
	
	
	void Update_LookAt();			// Target ��ġ�� �� Look���Ͱ��� ������ ����ؼ� ������

private:
	class CChr_Battle_Sazh* m_pChr_Battle_Sazh = { nullptr };

	STATE		m_eState = { STATE_END };
	MOVEMENT	m_eMovement = { STRAIGHT };
	_bool		m_isHurt = { false };

	_float		m_fDegree = { 0.f };

public:
	static	CChr_Battle_Sazh_State_Idle* Create(class CChr_Battle_Sazh* pChr_Battle_Light);
	virtual void Free() override;

};

END
