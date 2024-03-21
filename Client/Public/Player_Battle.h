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
class CUI_Pnal;
class CUI_Pnal_Attack;
class CMonster;
class CPlayer_Study;
class CInventory;
class CAbility;

class CPlayer_Battle final : public CBase, public CObserver_Handler
{
public:
	enum UISTAGE  { STAGE_SELECT, STAGE_TARGET, STAGE_COMMAND, STAGE_ITEM, STAGE_WAIT, STAGE_END };
private:
	CPlayer_Battle();
	~CPlayer_Battle() = default;

public:
	void Start();			// ���� ����Ʈ : troup�� ���� ������, �÷��̾� ����Ʈ, ������ ����Ʈ ���δ� ��������
	void Tick(_float fTimeDelta);
	HRESULT Render();		// ImGui �޾Ƽ� ����� �뵵
	
public:
	UISTAGE					Get_Stage() { return m_eStage; }
	CChr_Battle_Light*			Get_Leader() { return m_pLeader; }
	vector<CChr_Battle*>	Get_Members() { return m_Memebers; }
	vector<CPlayer_Study*>	Get_Monsters() { return m_Monsters; }
	CInventory*				Get_Inventory() { return m_pInventory; }
	CAbility*				Get_Ability() { return m_pAbility; }
	_float3					Get_CursorPosition() { return m_vCursorPosition; }

	_bool					Get_Command_empty() { return m_Commands.empty(); }
	size_t					Get_Command_size() { return m_Commands.size(); }

public:
	void Set_CursorPosition(_float3 vCursorPosition);

	void Change_Stage(UISTAGE eStage);
	void Back_Stage();

	void Check_Command_Insert(_uint iCost);
	_bool Check_Command_Full();
	void Add_Command(CUI_Pnal_Attack* pPnal_Attack);
	void Cancel_Command();								// Ŀ�ǵ� ������ ��� -> �ڿ��� ���� ������
	void Use_Command();									// Ŀ��尡 ���� -> ������ �������� ���

	void Set_Leader_Action();							// dequeue ������ ������ Leader���� ����� ����

private:
	virtual void NotifyObserver();

	HRESULT	Initialize();
	HRESULT	Add_Component_FSM();
	//void	Update_FSMState();		// ��� ���� ����?
	void	Update_Monsters();
	void	Update_Command();
	void	Update_CommandCost();
	void	Update_CommandPosition();

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CFSM*			m_pFSMCom = { nullptr };

	stack<UISTAGE>	m_PrevStage;

	UISTAGE					m_eStage = { STAGE_END };
	CChr_Battle_Light*		m_pLeader = { nullptr };		// Leader�� ���� ��������
	vector<CChr_Battle*>	m_Memebers;						// Leader �̿��� AI��
	vector<CPlayer_Study*>	m_Monsters;						// ���⵵ ���� �ؾ� �� 
	CInventory*				m_pInventory = { nullptr };		// �κ��丮
	CAbility*				m_pAbility = { nullptr };		// ��ų�� 
	_int					m_iCommandCost = { 0 };			// ����� ���� ���
	deque<CUI_Pnal_Attack*>	m_Commands;						// ���� ��ɵ�					// ����� �ϼ��Ǹ� ������ ��
																							// deque ������ �������� queue�� ���� ����
	//CUI_Pnal_Item*		m_Command_Item;


	// ������ ������ Command ������ ����ֱ�?

	// ������ ����Ʈ�� �갡 �������		-> ������ ���� �޾ƿ;���
	// ���� ����Ʈ�� �갡 �������
	

	_float3					m_vCursorPosition = { 0.f,0.f,0.f };

public:
	static CPlayer_Battle* Create();
	virtual void Free() override;
};


END
