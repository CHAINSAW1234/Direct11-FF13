#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)

class CBoss_State_Skill_HellBlast final : public CFSM_State
{
	enum STATE { MOVE, SKILL, STATE_END };
private:
	CBoss_State_Skill_HellBlast(class CBoss* pBoss);
	virtual ~CBoss_State_Skill_HellBlast() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	void Move(_float fTimeDelta);
	void Skill(_float fTimeDelta);

private:
	class CBoss*		m_pBoss = { nullptr };
	class CUI_Skill*	m_pUI_Skill = { nullptr };
	STATE				m_eState = { STATE_END };

public:
	static	CBoss_State_Skill_HellBlast* Create(class CBoss* pBoss);
	virtual void Free() override;

};

END