#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_Run final : public CFSM_State
{
private:
	CChr_Battle_Light_Run(class CChr_Battle_Light* pChr_Field);
	virtual ~CChr_Battle_Light_Run() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Update_Animation();

private:
	class CChr_Battle_Light* m_pChr_Battle_Light = { nullptr };

public:
	static	CChr_Battle_Light_Run* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END
