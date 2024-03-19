#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CUI_Pnal;
class CPlayer_Battle;

class CUI_Battle_Stage_Item final : public CFSM_State
{
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

	virtual void Render() override;

private:
	HRESULT Initialize();

private:
	CPlayer_Battle*		m_pPlayer_Battle = { nullptr };
	vector<CUI_Pnal*>	m_Pnals;
	_uint				m_Cursor = { 0 };

public:
	static	CUI_Battle_Stage_Item* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END