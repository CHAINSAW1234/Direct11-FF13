#pragma once

#include "Client_Defines.h"
#include "Observer_Handler.h"
#include "Optima.h"

BEGIN(Engine)
class CGameInstance;
class CFSM;
END

BEGIN(Client)
class CChr_Battle_Light;
class CChr_Battle;
class CUI_Pnal_Attack;
class CUI_Pnal_Item;
class CUI_Chr;
class CUI_Chain;
class CMonster;
class CInventory;

class CPlayer_Battle final : public CBase, public CObserver_Handler
{
public:
	enum UISTAGE  { STAGE_SELECT, STAGE_TARGET, STAGE_COMMAND, STAGE_ITEM, STAGE_TARGET_MEMBER, STAGE_OPTIMA,
		STAGE_WAIT, STAGE_WAIT_OPTIMA, STAGE_BEGIN, STAGE_FINISH, STAGE_END };
private:
	CPlayer_Battle();
	~CPlayer_Battle() = default;

public:
	void Start();			// ���� ����Ʈ : troup�� ���� ������, �÷��̾� ����Ʈ, ������ ����Ʈ ���δ� ��������
	void Tick(_float fTimeDelta);
	HRESULT Render();		// ImGui �޾Ƽ� ����� �뵵
	
public:
	UISTAGE					Get_Stage() { return m_eStage; }
	CChr_Battle_Light*		Get_Leader() { return m_pLeader; }
	vector<CChr_Battle*>	Get_Members() { return m_Memebers; }
	vector<CMonster*>		Get_Monsters() { return m_Monsters; }
	CInventory*				Get_Inventory() { return m_pInventory; }
	CAbility*				Get_Ability() { return m_pAbility; }
	_float3					Get_CursorPosition() { return m_vCursorPosition; }
	_bool					Get_Command_empty() { return m_Commands.empty(); }
	size_t					Get_Command_size() { return m_Commands.size(); }

public:
	void Set_CursorPosition(_float3 vCursorPosition);
	/*=========================FSM ����/*=========================*/
	void Change_Stage(UISTAGE eStage);
	void Back_Stage();

	/*=========================Ŀ�ǵ� ����=========================*/
	_int Get_Command_Cost() { return m_iCommandCost; }
	void Check_Command_Insert(_uint iCost);
	_bool Check_Command_Full();
	void Add_Command(CUI_Pnal_Attack* pPnal_Attack);
	void Cancel_Command();								// Ŀ�ǵ� ������ ��� -> �ڿ��� ���� ������
	void Use_Command();									// Ŀ��尡 ���� -> ������ �������� ���
	void Set_Command_Render(_bool isRender);

	/*=========================ITEM ����=========================*/
	_bool Check_Item();
	void  Add_Item(CUI_Pnal_Item* pPnal_Item);
	void  Cancel_Item();
	void  Use_Item();

	/*=========================OPTIMA ����=========================*/
	HRESULT				Create_Optima();
	COptima::Optima*	Get_Current_Optima();
	COptima::Optima*	Get_Optima(_int iOptimaIndex) { return m_pOptima->Get_Optima(iOptimaIndex); }
	size_t				Get_Optima_Size() { return m_pOptima->Get_Optima_Size(); }
	_int				Get_Current_Optima_Num() { return m_pOptima->Get_Current_Optima_Num(); }
	wstring				Get_Current_Optima_Name() { return m_pOptima->Get_Current_Optima_Name(); }
	wstring				Get_Optima_Name(_int iOptimaIndex) { return m_pOptima->Get_Optima_Name(iOptimaIndex); }
	HRESULT				Change_Optima(_int iOptimaIndex);

	void Set_Leader_Target(CGameObject* pTargetObject);
	void Set_Leader_Command();							// dequeue ������ ������ Leader���� ����� ����
	void Set_Leader_Item();
	void Check_Leader_Action();

	void Set_Member_Target(CGameObject* pTargetObject);
	void Change_Chain_Target(CGameObject* pTargetObject);
	void Set_Chain_Move();

private:
	virtual void NotifyObserver();

	HRESULT	Initialize();
	HRESULT	Add_Component_FSM();
	void	Create_UI();			// �ٸ� UI�� ����

	void	Update_FSMState();		// ��� ���� ����?
	void	Update_Monsters();
	void	Update_Command();
	void	Update_CommandCost();
	void	Update_CommandPosition();
	void	Check_Finish();

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CFSM*			m_pFSMCom = { nullptr };

	stack<UISTAGE>	m_PrevStage;

	UISTAGE					m_eStage = { STAGE_END };
	CChr_Battle_Light*		m_pLeader = { nullptr };		// Leader�� ���� ��������
	vector<CChr_Battle*>	m_Memebers;						// Leader �̿��� AI��
	vector<CMonster*>		m_Monsters;						// ���⵵ ���� �ؾ� �� 
	CInventory*				m_pInventory = { nullptr };		// �κ��丮
	COptima*				m_pOptima = { nullptr };		// ��Ƽ��
	CAbility*				m_pAbility = { nullptr };		// ��ų�� 
	_int					m_iCommandCost = { 0 };			// ����� ���� ���
	deque<CUI_Pnal_Attack*>	m_Commands;						// ���� ��ɵ�					// ����� �ϼ��Ǹ� ������ ��
																							// deque ������ �������� queue�� ���� ����
	CUI_Pnal_Item*			m_pCommand_Item = { nullptr };
	vector<CUI_Chr*>		m_pUI_Chr;						// ĳ������ ���¸� �����ִ� UI
															// ���� ���� ������ ���ؼ� ���� ����������
	CUI_Chain*				m_pUI_Chain = { nullptr };		// Ÿ���� Chain ������ �����ִ� UI, ��ġ �̵��� ���� ���� ���� ����

	// ������ ������ Command ������ ����ֱ�?

	// ������ ����Ʈ�� �갡 �������		-> ������ ���� �޾ƿ;���
	// ���� ����Ʈ�� �갡 �������
	

	_float3					m_vCursorPosition = { 0.f,0.f,0.f };

public:
	static CPlayer_Battle* Create();
	virtual void Free() override;
};


END
