#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CUI_Pnal;
class CPlayer_Battle;

class CUI_Battle_Stage_Command final : public CFSM_State
{
private:
	CUI_Battle_Stage_Command(CPlayer_Battle* pPlayer_Battle);
	virtual ~CUI_Battle_Stage_Command() = default;

public:
	// 상태 진입했을 때 처음만 호출
	virtual void OnStateEnter() override;
	// 상태 진입되어 있는 상태에서 매 tick마다 호출
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// 상태가 변경될 때 호출
	virtual void OnStateExit() override;

	virtual void Start() override;

private:
	HRESULT Add_Pnals();
	void	Update_Pnals();	// Optima 변경에 대비해서 OnStateEnter에서 호출하게 처리 -> Optima 변경 구현 이후 수정
	void	Update_Cursor();
	void	Update_KeyInput();
	void	Create_Pnal_Attack();

private:
	CPlayer_Battle*		m_pPlayer_Battle = { nullptr };
	vector<CUI_Pnal*>	m_Pnals;
	_int				m_iPnalCount = { 0 };	// 사용가능한 패널의 개수
	_int				m_iCursor = { 0 };

public:
	static	CUI_Battle_Stage_Command* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END