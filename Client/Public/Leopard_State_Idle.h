#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CLeopard_State_Idle final : public CFSM_State
{
	enum State { IDLE, MOVE, MOVE_BACK, TURN, STATE_END};
private:
	CLeopard_State_Idle(class CLeopard* m_pLeopard);
	virtual ~CLeopard_State_Idle() = default;

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
	void Move_Back(_float fTimeDelta);
	void Turn(_float fTimeDelta);

	void Update_LookAt();

private:
	class CLeopard* m_pLeopard = { nullptr };
	State			m_eState = { STATE_END};
	_float			m_fDegree = { 0.f };
	_float			m_fTimeDelta = { 0.f };
	_float			m_fPrevTimeDelta = { 0.f };
public:
	static	CLeopard_State_Idle* Create(class CLeopard* m_pLeopard);
	virtual void Free() override;

};

END