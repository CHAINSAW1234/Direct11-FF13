#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CChr_Field_State_Battle_Begin final : public CFSM_State
{
private:
	CChr_Field_State_Battle_Begin(class CChr_Field* pChr_Field);
	virtual ~CChr_Field_State_Battle_Begin() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	class CChr_Field* m_pChr_Field = { nullptr };

public:
	static	CChr_Field_State_Battle_Begin* Create(class CChr_Field* pChr_Field);
	virtual void				  Free() override;
};

END
