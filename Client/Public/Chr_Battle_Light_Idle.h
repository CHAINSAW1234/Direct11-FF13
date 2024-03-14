#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_Idle final : public CFSM_State
{
	enum STATE { IDLE, HURT, STATE_END };
private:
	CChr_Battle_Light_Idle(class CChr_Battle_Light* pChr_Field);
	virtual ~CChr_Battle_Light_Idle() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Idle();
	void Hurt();

private:
	class CChr_Battle_Light* m_pChr_Battle_Light = { nullptr };
	STATE m_eState = { STATE_END };
	_bool m_isHurt = { false };
public:
	static	CChr_Battle_Light_Idle* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END
