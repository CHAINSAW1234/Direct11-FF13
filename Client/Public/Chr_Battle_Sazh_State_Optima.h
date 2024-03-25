#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CChr_Battle_Sazh_State_Optima final : public CFSM_State
{
private:
	CChr_Battle_Sazh_State_Optima(class CChr_Battle_Sazh* pChr_Battle_Sazh);
	virtual ~CChr_Battle_Sazh_State_Optima() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	class CChr_Battle_Sazh* m_pChr_Battle_Sazh = { nullptr };

public:
	static	CChr_Battle_Sazh_State_Optima* Create(class CChr_Battle_Sazh* pChr_Battle_Sazh);
	virtual void Free() override;

};
END