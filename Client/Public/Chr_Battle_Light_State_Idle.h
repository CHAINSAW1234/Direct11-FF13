#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_State_Idle final : public CFSM_State
{
	enum STATE { IDLE, HURT, MOVE, ATTACK_READY, TURN, STATE_END };					// TURN ���߿� ����
	enum MOVEMENT { BACKWARD, LEFT, RIGHT, STRAIGHT, MOVEMENT_END};

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
	void Turn(_float fTimeDelta);

	void Change_State(STATE eState);
	void Change_MovementAnimation();
	void Update_Hurt();
	_float4 Cal_Line();			// ���������� Ÿ�������� ������ �������� ���Ѵ�
	_float4 Cal_Dir_Start_To_Target();
	_float4 Cal_Current_Direction();	// �̵� ������ ����ؼ� �ش�
private:
	class CChr_Battle_Light* m_pChr_Battle_Light = { nullptr };

	STATE		m_eState = { STATE_END };
	MOVEMENT	m_eMovement = { STRAIGHT };
	_bool		m_isHurt = { false };

	_float		m_fDegree = { 0.f };
	_float		m_fDist = { 5.f };

	_float		m_fTimeDelta = 0.f;
//	_float		m_fTurnCheckTimeDelta = { 0.f };		// ���� üũ�� �ð� ��� ����
//	_float		m_fMoveTimeDelta = { 0.f };			// �̵� �ð��� �ʹ� ��� �ȵ� 

	_int		m_iPatternCount = { 0 };			// ���� ��Ұ� ���Ե� �̵��� �ִ� Ƚ���� ���, 4�� �̻��� ��� startpoint�� �̵��ϰ� ó��
	
	_float		m_fStateTime = { 0.f };
	_float		m_fPrevTimeDelta = { 0.f };
	
	_bool		m_isMoveTurn = { false };


public:
	static	CChr_Battle_Light_State_Idle* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END

