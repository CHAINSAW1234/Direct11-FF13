#pragma once

#include "Client_Defines.h"
#include "Observer_Handler.h"

BEGIN(Engine)
class CGameInstance;
class CFSM;
END

BEGIN(Client)
class CChr_Battle_Light;
class CChr_Battle;
class CMonster;
//class CItem;
class CPlayer_Battle final : public CBase, public CObserver_Handler
{
public:
	enum UIStage  { STAGE_SELECT, STAGE_TARGET, STAGE_COMMAND, STAGE_ITEM, STAGE_END };
private:
	CPlayer_Battle();
	~CPlayer_Battle() = default;

public:
	void Start();			// ���� ����Ʈ : troup�� ���� ������, �÷��̾� ����Ʈ, ������ ����Ʈ ���δ� ��������
	void Tick(_float fTimeDelta);
	HRESULT Render();		// ImGui �޾Ƽ� ����� �뵵
	
public:
	UIStage	Get_Stage() { return m_eStage; }
	CChr_Battle* Get_Leader() { return m_pLeader; }
	vector<CChr_Battle*> Get_Members() { return m_Memebers; }
	void Change_Stage(UIStage eStage);

private:
	HRESULT Initialize();
	HRESULT Add_Component_FSM();
	void Update_FSMState();		// ��� ���� ����?

private:
	CGameInstance* m_pGameInstance = { nullptr };
	CFSM*			m_pFSMCom = { nullptr };

	UIStage					m_eStage = { STAGE_END };
	CChr_Battle*			m_pLeader = { nullptr };			// Leader�� ���� ��������
	vector<CChr_Battle*>	m_Memebers;				// Leader �̿��� AI��
	vector<CMonster*>		m_Monsters;;
	//vector<CItem*> m_Items;
	// ������ ����Ʈ�� �갡 �������
	// ���� ����Ʈ�� �갡 �������
	// 
public:
	static CPlayer_Battle* Create();
	virtual void Free() override;
};


END
