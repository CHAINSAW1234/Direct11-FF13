#pragma once
#include "Client_Defines.h"
#include "Monster.h"

BEGIN(Client)

class CLeopard final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK, STATE_HIT, STATE_FIELD, STATE_START, STATE_END };
	enum ANIMATION_LEOPARD {
		ATTACK, BATTLE_IDLE, BATTLE_START, FIELD_IDLE,
		HIT, HIT_RUN,
		MOVE_BACK_IDLE, MOVE_BACK_START, MOVE_BACK_STOP_LEFT, MOVE_BACK_STOP_RIGHT,
		MOVE_STRAIGHT_IDLE, MOVE_STRAIGHT_START, MOVE_STRAIGHT_STOP_LEFT, MOVE_STRAIGHT_STOP_RIGHT,
		RUN_IDLE, RUN_START, RUN_STOP, 
		TURN_JUMP_LEFT, TURN_JUMP_RIGHT, TURN_LEFT, TURN_RIGHT, ANIMATION_END};

private:
	CLeopard(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLeopard(const CMonster& rhs);
	virtual ~CLeopard() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	virtual void Set_Hit(_int iDamage) override;
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_LEOPARD iAnimationIndex, _bool isLoop);

private:
	STATE	m_eState = { STATE_END };

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;

public:
	static CLeopard* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END