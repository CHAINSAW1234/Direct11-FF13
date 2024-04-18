#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CFSM;
END


BEGIN(Client)
class CWeapon_Anim;
class CChr_Field final : public CGameObject
{
public:
	enum STATE { IDLE, WALK, MOVE, ITEM, BATTLE_BEGIN, STATE_END };
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
	enum ANIMATION_CHR_LIGHT_WEAPON {
		WEAPON_CLOSE, WEAPON_CLOSE_IDLE, WEAPON_OPEN, WEAPON_OPEN_IDLE
	};

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
	virtual void Start() override;

private:
	CModel*			m_pModelCom = { nullptr };
	CShader*		m_pShaderCom = { nullptr };
	CCollider*		m_pColliderCom = { nullptr };
	CFSM*			m_pFSMCom = { nullptr };
	CNavigation*	m_pNavigationCom = { nullptr };

	CWeapon_Anim*	m_pWeapon = { nullptr };

	STATE			m_eState = { STATE_END };
	_bool			m_isControl = { true };				// 조작 가능 여부 -> 아이템 조작, 전투시 false 처리

#ifdef _DEBUG
	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };
#endif

public: 
	CTransform* Get_Transform() { return m_pTransformCom; }
	CCollider*	Get_Collider() { return m_pColliderCom; }
	CNavigation* Get_Navigation() { return m_pNavigationCom; }
	_uint		Get_CurrentAnimationIndex() { return m_pModelCom->Get_CurrentAnimationIndex(); }
	_float		Get_CurrentTrackPosition() { return m_pModelCom->Get_CurrentTrackPosition(); }
	_bool		Is_Animation_Finished() { return m_pModelCom->isFinished(); }
	void		Set_TrackPosition(_float fTrackPosition) { m_pModelCom->Set_TrackPosition(fTrackPosition); }

public:
	HRESULT Change_State(STATE eState);
	void	Set_State_Battle_Start();
	void	Open_Level();
	void	Change_Animation(ANIMATION_CHR_FIELD iAnimationIndex, _bool isLoop);
	void	Change_Animation_Weapon(ANIMATION_CHR_LIGHT_WEAPON iAnimationIndex);

	_float4 Cal_Target_Direction();			// 카메라의 방향과 키입력을 바탕으로 이동 방향을 결정해서 리턴
	_float4	Get_Look();						// Player의 Look vector를 Y값을 지우고 리턴

private:
	HRESULT Add_Components();
	HRESULT Add_Component_FSM();
	HRESULT Add_Weapon();
	HRESULT Bind_ShaderResources();

	void	Update_FSMState(_float fTimeDelta);

#ifdef _DEBUG
	void	Show_ImGUI();
#endif

public:
	static CChr_Field* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END