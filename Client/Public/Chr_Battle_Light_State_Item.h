#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_State_Item final : public CFSM_State
{
private:
	CChr_Battle_Light_State_Item(class CChr_Battle_Light* pChr_Battle_Light);
	virtual ~CChr_Battle_Light_State_Item() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	class CChr_Battle_Light* m_pChr_Battle_Light = { nullptr };

public:
	static	CChr_Battle_Light_State_Item* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END