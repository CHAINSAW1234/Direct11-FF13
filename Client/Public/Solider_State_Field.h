#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CSolider_State_Field final : public CFSM_State
{
	enum STATE { IDLE, MOVE, TURN, STATE_END };

private:
	CSolider_State_Field(class CSolider* pSolider);
	virtual ~CSolider_State_Field() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Idle(_float fTimeDelta);
	void Move(_float fTimeDelta);
	void Turn(_float fTimeDelta);
	void Change_State(STATE eState);
	_float Cal_Next_Degree();


private:
	class CSolider* m_pSolider = { nullptr };
	STATE			m_eState = { STATE_END };
	_float			m_fDegree = { 0.f };
	_float			m_fTimeDelta = { 0.f };
	_float			m_fStateTime = { 0.f };
	_float			m_fPrevTimeDelta = { 0.f };

public:
	static	CSolider_State_Field* Create(class CSolider* pSolider);
	virtual void Free() override;

};

END