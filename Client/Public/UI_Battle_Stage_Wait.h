#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CUI_Pnal;
class CPlayer_Battle;

class CUI_Battle_Stage_Wait final : public CFSM_State
{
private:
	CUI_Battle_Stage_Wait(CPlayer_Battle* pPlayer_Battle);
	virtual ~CUI_Battle_Stage_Wait() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

private:
	void	Update_KeyInput();

private:
	CPlayer_Battle* m_pPlayer_Battle = { nullptr };

public:
	static	CUI_Battle_Stage_Wait* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END