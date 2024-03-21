#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CUI_Pnal;
class CPlayer_Battle;

class CUI_Battle_Stage_Item final : public CFSM_State
{
	const _float4 vItemColor = { 0.f,1.f,0.f,1.f };
private:
	CUI_Battle_Stage_Item(CPlayer_Battle* pPlayer_Battle);
	virtual ~CUI_Battle_Stage_Item() = default;

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
	void Update_Cursor();

private:
	CPlayer_Battle*		m_pPlayer_Battle = { nullptr };
	vector<CUI_Pnal*>	m_Pnals;
	_int				m_iCursor = { 0 };

public:
	static	CUI_Battle_Stage_Item* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END