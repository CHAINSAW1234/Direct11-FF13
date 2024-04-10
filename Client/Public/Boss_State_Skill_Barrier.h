#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CBoss_State_Skill_Barrier final : public CFSM_State
{
private:
	CBoss_State_Skill_Barrier(class CBoss* pBoss);
	virtual ~CBoss_State_Skill_Barrier() = default;

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
	class CBoss*		m_pBoss = { nullptr };
	class CUI_Skill*	m_pUI_Skill = { nullptr };
	_bool				m_isSkill = { false };

public:
	static	CBoss_State_Skill_Barrier* Create(class CBoss* pBoss);
	virtual void Free() override;

};

END