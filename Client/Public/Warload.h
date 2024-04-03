#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)
class CWarload final: public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_HIT, STATE_SKILL, STATE_FIELD, STATE_START, STATE_END };
	enum ANIMATION_WARLOAD {
		ATTACK, BATTLE_IDLE, BATTLE_START, FIELD_IDLE,
		HIT, JUMP_BACK,
		MOVE_BACK_IDLE, MOVE_BACK_STOP_LEFT, MOVE_BACK_STOP_RIGHT,
		MOVE_STRAIGHT_IDLE, MOVE_STRAIGHT_STOP_LEFT, MOVE_STRAIGHT_STOP_RIGHT,
		RUN_IDLE, RUN_START,
		SKILL_END, SKILL_HEAL_END, SKILL_HEAL_IDLE, SKILL_HEAL_START, SKILL_IDLE, SKILL_START,
		TURN_LEFT, TURN_RIGHT, ANIMATION_END
	};

private:
	CWarload(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWarload(const CMonster& rhs);
	virtual ~CWarload() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	_bool Get_IsJumpNext() { return m_isJumpNext; }

	void Set_IsJumpNext(_bool isJumpNext) { m_isJumpNext = isJumpNext; }

	virtual void Set_Hit(_int iDamage) override;
	virtual void Set_State_Battle_Start() override;
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_WARLOAD iAnimationIndex, _bool isLoop);

private:
	STATE	m_eState = { STATE_END };
	_bool	m_isJumpNext = { false };			// 공격 이후 반드시 백점프
	_bool	m_isUseSkill = { false };			// 반피일때 스킬 한번만 사용
private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;

public:
	static CWarload* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END