#pragma once
#include "Client_Defines.h"
#include "Chr_Battle.h"

BEGIN(Client)
class CChr_Battle_Vanila final : public CChr_Battle
{
public:
	enum STATE { IDLE, SKILL, HEAL, HIT, DEAD, FINISH, OPTIMA, STATE_END };
	enum ANIMATION_CHR_BATTLE_VANILA {	// FINISH -> ÀüÅõ Á¾·á	// IDLE_IDLEÀº STATE¶û °ãÃ³¼­ ¸¸µë
		DEAD_END, DEAD_IDLE, DEAD_START, 
		HIT_FALL,  HIT_FALL_WAKE, HIT_FALL_DEAD,
		ANIM_FINISH, ANIM_HEAL, HIT_BACK, HIT_LEFT, HIT_RIGHT, HIT_STRAIGTH,
		HURT_END, HURT_IDLE, HURT_START, ANIM_IDLE,
		MOVE_BACK_IDLE, MOVE_BACK_STOP_LEFT, MOVE_BACK_STOP_RIGHT,
		MOVE_LEFT_IDLE, MOVE_LEFT_STOP_LEFT, MOVE_LEFT_STOP_RIGHT, 
		MOVE_RIGHT_IDLE, MOVE_RIGHT_STOP_LEFT, MOVE_RIGHT_STOP_RIGHT,
		MOVE_STRAIGHT_IDLE, MOVE_STRAIGHT_START, MOVE_STRAIGHT_STOP_LEFT, MOVE_STRAIGHT_STOP_RIGHT,
		OPTIMACHANGE, ANIM_SKILL, ANIM_SKILL2, TURN_LEFT, TURN_RIGHT, ANIMATION_END
	};
	enum ANIMATION_CHR_BATTLE_VANILA_WEAPON {
		BATTLE_END, WEAPON_IDLE, ANIMATION_WEAPON_END
	};

private:
	CChr_Battle_Vanila(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChr_Battle_Vanila(const CChr_Battle_Vanila& rhs);
	virtual ~CChr_Battle_Vanila() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void Start() override;

public:
	HRESULT			Change_State(STATE eState);
	void			Change_Animation(ANIMATION_CHR_BATTLE_VANILA iAnimationIndex, _bool isLoop);
	void			Change_Animation_Weapon(ANIMATION_CHR_BATTLE_VANILA_WEAPON iAnimationIndex, _bool isLoop);
	virtual void	Change_Role(CAbility::ROLE eRole) override;
	virtual void	Set_State_Battle_Finish() override;
	virtual void	Set_Target(CGameObject* pTargetObject) override;
	virtual void	Set_Hit(_int iDamage);								// ÇÇ°Ý »óÅÂ·Î º¯°æ
	virtual void	Add_Hp(_int iHp) override;
	virtual void	Update_Target() override;									// Å¸°Ù »ç¸Á½Ã º¯°æ
	virtual void	Create_Sphere(_int iDamage, _int iWeaponNum = 0) override;
	void			Create_Sphere_Heal();

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;
	virtual HRESULT Add_PartObjects() override;
	virtual HRESULT Add_Ability() override;

	void			Update_HealTarget();


private:
	STATE		m_eState = { STATE_END };

#ifdef DEBUG
	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };
#endif

public:
	static CChr_Battle_Vanila* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END