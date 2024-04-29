#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)
class CWeapon_Anim;
class CSolider : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_HIT, STATE_SKILL, STATE_FIELD, STATE_START, STATE_END };
	enum ANIMATION_SOLIDER {
		ATTACK1, ATTACK2, ATTACK3, BATTLE_IDLE, BATTLE_START, 
		FIELD_IDLE, FIELD_IDLE2, FIELD_IDLE3,
		FIELD_MOVE_IDLE, FIELD_MOVE_START, FIELD_MOVE_STOP, FIELD_TURN_LEFT, FIELD_TURN_RIGHT,
		HIT, JUMP_BACK,
		MOVE_BACK_IDLE, MOVE_BACK_START, MOVE_BACK_STOP_LEFT, MOVE_BACK_STOP_RIGHT,
		MOVE_LEFT_IDLE, MOVE_LEFT_START, MOVE_LEFT_STOP_LEFT, MOVE_LEFT_STOP_RIGHT,
		MOVE_RIGHT_IDLE, MOVE_RIGHT_START, MOVE_RIGHT_STOP_LEFT, MOVE_RIGHT_STOP_RIGHT,
		MOVE_STRAIGHT_IDLE, MOVE_STRAIGHT_START, MOVE_STRAIGHT_STOP_LEFT, MOVE_STRAIGHT_STOP_RIGHT,
		RUN_IDLE, RUN_START, RUN_STOP,
		TURN_LEFT, TURN_RIGHT, ANIMATION_END
	};

protected:
	CSolider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSolider(const CSolider& rhs);
	virtual ~CSolider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	_bool Get_IsJumpNext() { return m_isJumpNext; }
	virtual _float4 Get_BonePos(const string strBoneName) override;

	void Set_IsJumpNext(_bool isJumpNext) { m_isJumpNext = isJumpNext; }

	virtual void Set_Hit(_int iDamage, _float fChain) override;
	virtual void Set_State_Battle_Start() override;
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_SOLIDER iAnimationIndex, _bool isLoop);

	virtual void PlaySound_Attack();
protected:
	CWeapon_Anim* m_pWeapon = { nullptr };
	STATE	m_eState = { STATE_END };
	_bool	m_isJumpNext = { false };			// 공격 이후 반드시 백점프

protected:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;
	virtual HRESULT Add_PartObjects();

public:
	static CSolider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END