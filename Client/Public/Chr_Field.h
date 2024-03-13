#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CFSM;
END


BEGIN(Client)

class CChr_Field final : public CGameObject
{
private:
	enum STATE { IDLE, WALK, MOVE, ITEM, BATTLE_BEGIN, STATE_END};
	enum ANIMATION_CHR_FIELD {
		BATTLE_BEGIN_IDLE, BATTLE_BEGIN_SURPRISE,
		IDLE_LEFT_INARM, IDLE_LEFT_INARM_TO_NOR, IDLE_LEFT_INARM_TO_RIGHT,
		IDLE_NOR, IDLE_NOR_TO_LEFT_INARM, IDLE_RIGHT_INARM, IDLE_RIGHT_INARM_TO_LEFT,
		IDLE_TURN_LEFT, IDLE_TURN_RIGHT,
		ITEM_END, ITEM_IDLE, ITEM_START,
		MOVE_IDLE, MOVE_START, MOVE_START_WITH_TURN_LEFT, MOVE_START_WITH_TURN_RIGHT,
		MOVE_STOP_LEFT, MOVE_STOP_RIGHT, MOVE_TURN_LEFT, MOVE_TURN_RIGHT,
		WALK_IDLE, WALK_START_WITH_TURN_LEFT, WALK_START_WITH_TURN_RIGHT,
		WALK_STOP_LEFT, WALK_STOP_RIGHT, ANIMATION_END };
private:
	CChr_Field(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChr_Field(const CChr_Field& rhs);
	virtual ~CChr_Field() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	friend class CChr_Field_State_Idle;
	friend class CChr_Field_State_Walk;

	CModel*		m_pModelCom = { nullptr };
	CShader*	m_pShaderCom = { nullptr };
	CFSM*		m_pFSMCom = { nullptr };

	STATE		m_eState = { STATE_END };

public:
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_CHR_FIELD iAnimationIndex, _bool isLoop);
private:
	HRESULT Add_Components();
	HRESULT Add_Component_FSM();
	HRESULT Bind_ShaderResources();

	void	Update_FSMState(_float fTimeDelta);

public:
	static CChr_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END