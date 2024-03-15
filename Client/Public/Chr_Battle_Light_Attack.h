#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_Attack final: public CFSM_State
{
private:
	CChr_Battle_Light_Attack(class CChr_Battle_Light* pChr_Battle_Light);
	virtual ~CChr_Battle_Light_Attack() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	class CChr_Battle_Light_Attack* m_pChr_Battle_Light = { nullptr };

public:
	static	CChr_Battle_Light_Attack* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END
