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
	void Start();			// 몬스터 리스트 : troup로 따로 관리됨, 플레이어 리스트, 아이템 리스트 전부다 갖고있음
	void Tick(_float fTimeDelta);
	HRESULT Render();		// ImGui 달아서 사용할 용도
	
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
	/*=========================FSM 관련/*=========================*/
	void Change_Stage(UISTAGE eStage);
	void Back_Stage();

	/*=========================커맨드 관련=========================*/
	_int Get_Command_Cost() { return m_iCommandCost; }
	void Check_Command_Insert(_uint iCost);
	_bool Check_Command_Full();
	void Add_Command(CUI_Pnal_Attack* pPnal_Attack);
	void Cancel_Command();								// 커맨드 예약을 취소 -> 뒤에서 부터 삭제됨
	void Use_Command();									// 커멘드가 사용됨 -> 공격이 수행됬을 경우
	void Set_Command_Render(_bool isRender);

	/*=========================ITEM 관련=========================*/
	_bool Check_Item();
	void  Add_Item(CUI_Pnal_Item* pPnal_Item);
	void  Cancel_Item();
	void  Use_Item();

	/*=========================OPTIMA 관련=========================*/
	HRESULT				Create_Optima();
	COptima::Optima*	Get_Current_Optima();
	COptima::Optima*	Get_Optima(_int iOptimaIndex) { return m_pOptima->Get_Optima(iOptimaIndex); }
	size_t				Get_Optima_Size() { return m_pOptima->Get_Optima_Size(); }
	_int				Get_Current_Optima_Num() { return m_pOptima->Get_Current_Optima_Num(); }
	wstring				Get_Current_Optima_Name() { return m_pOptima->Get_Current_Optima_Name(); }
	wstring				Get_Optima_Name(_int iOptimaIndex) { return m_pOptima->Get_Optima_Name(iOptimaIndex); }
	HRESULT				Change_Optima(_int iOptimaIndex);

	void Set_Leader_Target(CGameObject* pTargetObject);
	void Set_Leader_Command();							// dequeue 정보를 바탕을 Leader에게 명령을 전달
	void Set_Leader_Item();
	void Check_Leader_Action();

	void Set_Member_Target(CGameObject* pTargetObject);
	void Change_Chain_Target(CGameObject* pTargetObject);
	void Set_Chain_Move();

private:
	virtual void NotifyObserver();

	HRESULT	Initialize();
	HRESULT	Add_Component_FSM();
	void	Create_UI();			// 다른 UI들 생성

	void	Update_FSMState();		// 사용 안할 지도?
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
	CChr_Battle_Light*		m_pLeader = { nullptr };		// Leader는 따로 갖고있자
	vector<CChr_Battle*>	m_Memebers;						// Leader 이외의 AI들
	vector<CMonster*>		m_Monsters;						// 여기도 변경 해야 함 
	CInventory*				m_pInventory = { nullptr };		// 인벤토리
	COptima*				m_pOptima = { nullptr };		// 옵티마
	CAbility*				m_pAbility = { nullptr };		// 스킬셋 
	_int					m_iCommandCost = { 0 };			// 저장된 공격 명령
	deque<CUI_Pnal_Attack*>	m_Commands;						// 공격 명령들					// 명령이 완성되면 전달할 것
																							// deque 정보를 바탕으로 queue를 만들어서 전달
	CUI_Pnal_Item*			m_pCommand_Item = { nullptr };
	vector<CUI_Chr*>		m_pUI_Chr;						// 캐릭터의 상태를 보여주는 UI
															// 렌더 상태 변경을 위해서 직접 가지고있음
	CUI_Chain*				m_pUI_Chain = { nullptr };		// 타겟의 Chain 정보를 보여주는 UI, 위치 이동을 위해 직접 갖고 있음

	// 아이템 사용시의 Command 별개로 들고있기?

	// 아이템 리스트는 얘가 들고있음		-> 생성시 따로 받아와야함
	// 몬스터 리스트도 얘가 들고있음
	

	_float3					m_vCursorPosition = { 0.f,0.f,0.f };

public:
	static CPlayer_Battle* Create();
	virtual void Free() override;
};


END
