#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CHellBlast;
class CBoss_State_Skill_HellBlast final : public CFSM_State
{
	enum STATE { MOVE, SKILL, STATE_END };
private:
	CBoss_State_Skill_HellBlast(class CBoss* pBoss);
	virtual ~CBoss_State_Skill_HellBlast() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Move(_float fTimeDelta);
	void Skill(_float fTimeDelta);

private:
	class CBoss*		m_pBoss = { nullptr };
	class CUI_Skill*	m_pUI_Skill = { nullptr };
	_bool				m_isSkill[2] = {false, false};
	STATE				m_eState = { STATE_END };
	CHellBlast*			m_pHellBlast = { nullptr };

public:
	static	CBoss_State_Skill_HellBlast* Create(class CBoss* pBoss);
	virtual void Free() override;

};

END