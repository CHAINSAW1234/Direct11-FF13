#pragma once

#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CChr_Field_State_Idle final : public CFSM_State
{
	enum STATE { WAIT, IDLE, LEFT_INARM, RIGHT_INARM, STATE_END};
private:
	CChr_Field_State_Idle(class CChr_Field* pChr_Field);
	virtual ~CChr_Field_State_Idle() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void	Update_Animation();

private:
	class CChr_Field*	m_pChr_Field = { nullptr };

	STATE				m_eState = { STATE_END };
	_float				m_fTimeDelta = { 0.f };	// for change motion

public:
	static	CChr_Field_State_Idle* Create(class CChr_Field* pChr_Field);
	virtual void				  Free() override;

};

END
