#pragma once
#include "Monster.h"

BEGIN(Client)
class CEffect;
class CBoss final : public CMonster
{
public:
	enum STATE { STATE_IDLE, STATE_ATTACK_PHYSIC, STATE_ATTACK_MAGIC, STATE_HIT, STATE_SKILL_BARRIER, STATE_SKILL_HELLBLAST, STATE_FIELD, STATE_END };
	enum PHASE { PHASE1, PHASE2, PHASE_END };
	enum ANIMATION_SOLIDER {
		ATTACK1, ATTACK2, HIT, HIT2, IDLE, JUMP_BACK,
		MOVE_STRAIGHT_IDLE, MOVE_STRAIGHT_START, MOVE_STRAIGHT_STOP,
		SKILL_BARRIER, SKILL_HELLBLAST, 
		TURN_LEFT, TURN_RIGHT, ANIMATION_END
	};

protected:
	CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	CModel* Get_Body() { return m_pModelCom; }
	void Set_Barrier(_bool isBarrier);

	virtual void Add_Chain(_float fChain) override;
	virtual void Set_Hit(_int iDamage, _float fChain) override;
	
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_SOLIDER iAnimationIndex, _bool isLoop);

	void Update_Pattern();
	void Clear_Pattern();

	virtual void PlaySound_Attack() override;

public:
	void Create_Electricity();

private:
	void Change_Phase();
	virtual void Update_Chain(_float fTimeDelta) override;

private:
	STATE	m_eState = { STATE_END };
	PHASE	m_ePhase = { PHASE_END };
	queue<STATE> m_Patterns;
	_bool  m_isBarrier = { false };
	vector<CEffect*> m_Barrier;

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END