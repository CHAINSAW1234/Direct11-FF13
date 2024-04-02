#pragma once
#include "Client_Defines.h"
#include "Chr_Battle.h"
#include "Inventory.h"

BEGIN(Client)
class CChr_Battle_Light final : public CChr_Battle
{
public:
	enum STATE { IDLE, ATTACK, ITEM, PREPARE, HIT, DEAD, OPTIMA, FINISH, STATE_END };
	enum ANIMATION_CHR_BATTLE_LIGHT {	// FINISH -> 전투 종료	// IDLE_IDLE은 STATE랑 겹처서 만듬
		ATTACK_AIR, ATTACK_AIR_SPIN, ATTACK_AIR_SPIN2, ATTACK_AMBUSH, ATTACK_AMBUSH2, ATTACK_AREABLAST,
		ATTACK_END, ATTACK_END2, ATTACK_NOR1, ATTACK_NOR2, ATTACK_NOR_3, ATTACK_NOR_4, ATTACK_PREPARE,
		DEAD_END, DEAD_IDLE, DEAD_START, ANIM_FINISH,
		HIT_AIR, HIT_AIR_START, HIT_BACK, HIT_FALL, HIT_FALL_DEAD, HIT_FALL_GETUP, HIT_LEFT, HIT_RIGHT,
		HIT_STRAIGHT_LEFT, HIT_STRAIGHT_RIGHT, ANIM_IDLE, IDLE_HURT, IDLE_HURT_START, IDLE_HURT_END,
		IDLE_TURN_LEFT, IDLE_TURN_RIGHT, ITEM_SELF,
		JUMP_FALL, JUMP_LAND, JUMP_START, JUMP_UP,
		MOVE_BACK, MOVE_BACK_STOP_LEFT, MOVE_BACK_STOP_RIGHT,
		MOVE_LEFT, MOVE_LEFT_STOP_LEFT, MOVE_LEFT_STOP_RIGHT,
		MOVE_RIGHT, MOVE_RIGHT_STOP_LEFT, MOVE_RIGHT_STOP_RIGHT,
		MOVE_STRAIGHT, MOVE_STRAIGHT_STOP_LEFT, MOVE_STRAIGHT_STOP_RIGHT,
		OPTIMACHANGE, RUN_IDLE, RUN_START, RUN_START_WITH_TURN_LEFT, RUN_START_WITH_TURN_RIGHT, RUN_STOP,
		SKILL, SKILL_AIR, 
		SKILL_AIR_END, SKILL_AIR_IDLE, SKILL_AIR_START,
		SKILL_2, SKILL_END, SKILL_IDLE, SKILL_START, ANIM_TP };
	enum ANIMATION_CHR_BATTLE_LIGHT_WEAPON {
		WEAPON_CLOSE, WEAPON_CLOSE_IDLE, WEAPON_OPEN, WEAPON_OPEN_IDLE
	};
	// idle_turn 2개 안쓰기로함
private:
	CChr_Battle_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChr_Battle_Light(const CChr_Battle_Light& rhs);
	virtual ~CChr_Battle_Light() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_CHR_BATTLE_LIGHT iAnimationIndex, _bool isLoop);
	void	Change_Animation_Weapon(ANIMATION_CHR_BATTLE_LIGHT_WEAPON iAnimationIndex);
	
	/*============================COMNMAND============================*/
	size_t	Get_Command_Size() { return m_pCommands->size(); }
	_int	Get_Command_Cost_Sum();
	_int	Get_Current_Command_Cost() { return m_pCommands->front().second; }
	virtual CRole::SKILL Get_Current_Command() override;
	virtual void Use_Command() override;
	void	Cancel_Command();
	void	Set_Command(deque<pair<CRole::SKILL, _int>>* pCommand);				// Player에게 명령을 전달

	/*============================ITEM============================*/
	CInventory::ITEM Get_Item() { return m_eItem; }
	void	Use_Item();
	void	Set_Item(CInventory::ITEM eItem) { m_eItem = eItem; }

	void	Determine_Action_Based_On_Command();						// queue에 의거하여 행동을 결정


	/*============================ABILITY============================*/
	virtual void Change_Role(CAbility::ROLE eRole) override;

	/*============================기타============================*/
	virtual void Set_Hit(_int iDamage);
	void   Check_Interact_Weapon();			// 무기의 충돌 체크 -> 상대 객체 -> 몬스터 피격 판정 주기 -> 타겟 객체만
	void   Check_Interact_Weapon_Multi();	// 무기의 충돌 체크 -> 상대 객체 -> 몬스터 피격 판정 주기 -> 주변 모든 객체

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;
	virtual HRESULT Add_PartObjects() override;
	virtual HRESULT Add_Ability() override;

	void	Update_FSMState(_float fTimeDelta);
	void	Show_ImGUI();

private:
	STATE		m_eState = { STATE_END };
	_bool		m_isControl = { true };				// 조작 가능 여부 -> 아이템 조작, 전투시 false 처리

	deque<pair<CRole::SKILL, _int>>*	m_pCommands = { nullptr };
	CInventory::ITEM		m_eItem = { CInventory::ITEM_END };
	class CImGUI_Manager*	m_pImGUI_Manager = { nullptr };

public:
	static CChr_Battle_Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END