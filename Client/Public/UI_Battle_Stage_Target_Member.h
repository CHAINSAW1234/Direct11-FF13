#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CUI_Pnal;
class CPlayer_Battle;

class CUI_Battle_Stage_Target_Member final : public CFSM_State
{
private:
	CUI_Battle_Stage_Target_Member(CPlayer_Battle* pPlayer_Battle);
	virtual ~CUI_Battle_Stage_Target_Member() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

	virtual void Start() override;

private:
	HRESULT Add_Pnals();
	void	Update_Cursor();
	void	Update_KeyInput();

private:
	CPlayer_Battle*		m_pPlayer_Battle = { nullptr };
	vector<CUI_Pnal*>	m_Pnals;
	_int				m_iCursor = { 0 };

public:
	static	CUI_Battle_Stage_Target_Member* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END