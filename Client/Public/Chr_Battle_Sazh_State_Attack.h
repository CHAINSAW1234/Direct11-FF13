#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Sazh_State_Attack final : public CFSM_State
{
private:
	CChr_Battle_Sazh_State_Attack(class CChr_Battle_Sazh* pChr_Battle_Light);
	virtual ~CChr_Battle_Sazh_State_Attack() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	void Attack();

private:
	class CChr_Battle_Sazh* m_pChr_Battle_Sazh = { nullptr };
	class CUI_Skill*		m_pUI_Skill = { nullptr };
	_bool					m_isCommandFinish = { false };
public:
	static	CChr_Battle_Sazh_State_Attack* Create(class CChr_Battle_Sazh* pChr_Battle_Light);
	virtual void Free() override;

};

END
