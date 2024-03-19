#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CUI_Pnal;
class CPlayer_Battle;

class CUI_Battle_Stage_Select final : public CFSM_State
{
	const wstring strPnalName[3] = { TEXT("����"), TEXT("Ŀ���"), TEXT("������") };
private:
	CUI_Battle_Stage_Select(CPlayer_Battle* pPlayer_Battle);
	virtual ~CUI_Battle_Stage_Select() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	HRESULT Initialize();
	HRESULT Add_Pnals();

	void	Change_Stage();

private:
	CPlayer_Battle*		m_pPlayer_Battle = { nullptr };
	vector<CUI_Pnal*>	m_Pnals;
	_int				m_Cursor = { 0 };

public:
	static	CUI_Battle_Stage_Select* Create(CPlayer_Battle* pPlayer_Battle);
	virtual void Free() override;

};

END