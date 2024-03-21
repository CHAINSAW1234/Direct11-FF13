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
	void Start();			// 몬스터 리스트 : troup로 따로 관리됨, 플레이어 리스트, 아이템 리스트 전부다 갖고있음
	void Tick(_float fTimeDelta);
	HRESULT Render();		// ImGui 달아서 사용할 용도
	
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
	void Cancel_Command();								// 커맨드 예약을 취소 -> 뒤에서 부터 삭제됨
	void Use_Command();									// 커멘드가 사용됨 -> 공격이 수행됬을 경우

	void Set_Leader_Action();							// dequeue 정보를 바탕을 Leader에게 명령을 전달

private:
	virtual void NotifyObserver();

	HRESULT	Initialize();
	HRESULT	Add_Component_FSM();
	//void	Update_FSMState();		// 사용 안할 지도?
	void	Update_Monsters();
	void	Update_Command();
	void	Update_CommandCost();
	void	Update_CommandPosition();

private:
	CGameInstance*	m_pGameInstance = { nullptr };
	CFSM*			m_pFSMCom = { nullptr };

	stack<UISTAGE>	m_PrevStage;

	UISTAGE					m_eStage = { STAGE_END };
	CChr_Battle_Light*		m_pLeader = { nullptr };		// Leader는 따로 갖고있자
	vector<CChr_Battle*>	m_Memebers;						// Leader 이외의 AI들
	vector<CPlayer_Study*>	m_Monsters;						// 여기도 변경 해야 함 
	CInventory*				m_pInventory = { nullptr };		// 인벤토리
	CAbility*				m_pAbility = { nullptr };		// 스킬셋 
	_int					m_iCommandCost = { 0 };			// 저장된 공격 명령
	deque<CUI_Pnal_Attack*>	m_Commands;						// 공격 명령들					// 명령이 완성되면 전달할 것
																							// deque 정보를 바탕으로 queue를 만들어서 전달
	//CUI_Pnal_Item*		m_Command_Item;


	// 아이템 사용시의 Command 별개로 들고있기?

	// 아이템 리스트는 얘가 들고있음		-> 생성시 따로 받아와야함
	// 몬스터 리스트도 얘가 들고있음
	

	_float3					m_vCursorPosition = { 0.f,0.f,0.f };

public:
	static CPlayer_Battle* Create();
	virtual void Free() override;
};


END
