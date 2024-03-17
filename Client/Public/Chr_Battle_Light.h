#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "PartObject.h"

BEGIN(Engine)
class CFSM;
class CModel;
class CShader;
class CPartObject;
END

BEGIN(Client)

class CChr_Battle_Light final : public CGameObject
{
public:
	enum STATE { IDLE, ATTACK, ITEM, HIT, DEAD, TP, STATE_END };
	enum ANIMATION_CHR_BATTLE_LIGHT {	// FINISH -> 전투 종료	// IDLE_IDLE은 STATE랑 겹처서 만듬
		ATTACK_AIR, ATTACK_AIR_SPIN, ATTACK_AIR_SPIN2, ATTACK_AMBUSH, ATTACK_AMBUSH2, ATTACK_AREABLAST,
		ATTACK_END, ATTACK_END2, ATTACK_NOR1, ATTACK_NOR2_2, ATTACK_NOR_3, ATTACK_PREPARE,
		DEAD_END, DEAD_IDLE, DEAD_START, FINISH,
		HIT_AIR, HIT_AIR_START, HIT_BACK, HIT_FALL, HIT_FALL_DEAD, HIT_FALL_GETUP, HIT_LEFT, HIT_RIGHT,
		HIT_STRAIGHT_LEFT, HIT_STRAIGHT_RIGHT, ANIM_IDLE, IDLE_HURT, IDLE_HURT_START, IDLE_HURT_END,
		IDLE_TURN_LEFT, IDLE_TURN_RIGHT, ITEM_OTHER, ITEM_SELF,
		JUMP_FALL, JUMP_LAND, JUMP_START, JUMP_UP,
		MOVE_BACK, MOVE_BACK_STOP_LEFT, MOVE_BACK_STOP_RIGHT,
		MOVE_LEFT, MOVE_LEFT_STOP_LEFT, MOVE_LEFT_STOP_RIGHT,
		MOVE_RIGHT, MOVE_RIGHT_STOP_LEFT, MOVE_RIGHT_STOP_RIGHT,
		MOVE_STRAIGHT, MOVE_STRAIGHT_STOP_LEFT, MOVE_STRAIGHT_STOP_RIGHT,
		OPTIMACHANGE, RUN_IDLE, RUN_START, RUN_START_WITH_TURN_LEFT, RUN_START_WITH_TURN_RIGHT, RUN_STOP,
		SKILL, SKILL_AIR, 
		SKILL_AIR_END, SKILL_AIR_IDLE, SKILL_AIR_START,
		SKILL_END, SKILL_IDLE, SKILL_START, ANIM_TP };
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

private:
	CGameObject* m_pTargetObject = { nullptr };

	//CModel*		m_pModelCom = { nullptr };
	//CShader*	m_pShaderCom = { nullptr };
	CFSM*		m_pFSMCom = { nullptr };

	STATE		m_eState = { STATE_END };
	_bool		m_isControl = { true };				// 조작 가능 여부 -> 아이템 조작, 전투시 false 처리

	vector<CPartObject*> m_PartObjects;				// PartObject를 보관 -> vector가 낫다고 판단, 무기 교체 가능성이 0에 수렴

	_int  m_AttackCount = { 0 };	// 1번의 공격에서의 공격 횟수 처리 최대 1~ 3 // 나중에 리스트로 변경 할 것 
	// 초기위치 필요한?

	_float4		m_vStartPosition = { 0.f,0.f,0.f,0.f };
	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	_uint		Get_CurrentAnimationIndex();
	_float		Get_CurrentTrackPosition();
	_bool		Is_Animation_Finished();
	void		Set_TrackPosition(_float fTrackPosition);

	_float4		Get_Target_Position() { return ((CTransform*)m_pTargetObject->Get_Component(g_strTransformTag))->Get_State_Float4(CTransform::STATE_POSITION); }
	_float4		Get_Start_Position() { return m_vStartPosition; }
	void		Set_Target(CGameObject* pTargetObject) { m_pTargetObject = pTargetObject; }

public:
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_CHR_BATTLE_LIGHT iAnimationIndex, _bool isLoop);
	void	Change_Animation_Weapon(ANIMATION_CHR_BATTLE_LIGHT_WEAPON iAnimationIndex);

	_float4	Get_Look();						// Player의 Look vector를 Y값을 지우고 리턴
	
private:
	HRESULT Add_Components();
	HRESULT Add_PartObjects();
	HRESULT Add_Component_FSM();

	void	Update_FSMState(_float fTimeDelta);
	void	Show_ImGUI();

public:
	static CChr_Battle_Light* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END