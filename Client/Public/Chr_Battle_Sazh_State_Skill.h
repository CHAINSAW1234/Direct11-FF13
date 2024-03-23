#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Sazh_State_Skill final : public CFSM_State
{
private:
	CChr_Battle_Sazh_State_Skill(class CChr_Battle_Sazh* pChr_Battle_Light);
	virtual ~CChr_Battle_Sazh_State_Skill() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Skill();

private:
	class CChr_Battle_Sazh* m_pChr_Battle_Sazh = { nullptr };
	_bool					m_isCommandFinish = { false };
public:
	static	CChr_Battle_Sazh_State_Skill* Create(class CChr_Battle_Sazh* pChr_Battle_Light);
	virtual void Free() override;

};

END
