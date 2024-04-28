#pragma once
#include "Client_Defines.h"
#include "Chr_Battle.h"

BEGIN(Client)
class CChr_Battle_Sazh final : public CChr_Battle
{
public:
	enum STATE { IDLE, ATTACK, SKILL, HIT, DEAD, OPTIMA, FINISH, STATE_END };
	enum ANIMATION_CHR_BATTLE_SAZH {	// FINISH -> 전투 종료	// IDLE_IDLE은 STATE랑 겹처서 만듬
		ATTACK_NOR1, ATTACK_NOR2, ATTACK_NOR3, BACK_END_ERROR,
		DEAD_END, DEAD_IDLE, DEAD_START, ANIM_FINISH, 
		HIT_BACK, HIT_FALL_DEAD, HIT_FALL_LAND, HIT_FALL_WAKE,
		HIT_LEFT, HIT_RIGHT, HIT_STRAIGTH,
		HURT_END, HURT_IDLE, HURT_START, ANIM_IDLE,
		MOVE_BACK_END_LEFT, MOVE_BACK_END_RIGHT, MOVE_BACK_IDLE, MOVE_BACK_START,
		MOVE_LEFT_END_LEFT, MOVE_LEFT_END_RIGHT, MOVE_LEFT_IDLE, MOVE_LEFT_START,
		MOVE_RIGHT_END_LEFT, MOVE_RIGHT_END_RIGHT, MOVE_RIGHT_IDLE, MOVE_RIGHT_START,
		MOVE_STRAIGHT_END_LEFT, MOVE_STRAIGHT_END_RIGHT, MOVE_STRAIGHT_IDLE, MOVE_STRAIGHT_START,
		OPTIMACHANGE, SKILL_NOR1, SKILL_NOR2, TURN_LEFT, TURN_RIGHT, ANIMATION_END
	};

private:
	CChr_Battle_Sazh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CChr_Battle_Sazh(const CChr_Battle_Sazh& rhs);
	virtual ~CChr_Battle_Sazh() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void	Tick(_float fTimeDelta) override;
	virtual HRESULT Late_Tick(_float fTimeDelta) override;
	virtual HRESULT Render() override;
	virtual void	Start() override;

public:
	HRESULT			Change_State(STATE eState);
	void			Change_Animation(ANIMATION_CHR_BATTLE_SAZH iAnimationIndex, _bool isLoop);
	virtual void	Change_Role(CAbility::ROLE eRole) override;
	virtual void	Set_State_Battle_Finish() override;
	virtual void	Set_Hit(_int iDamage);								// 피격 상태로 변경
	virtual void	Add_Hp(_int iHp) override;
	virtual void	Create_Sphere(_int iDamage, _int iWeaponNum = 0) override;
	void			Create_Bullet(_int iWeaponIndex);
private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;
	virtual HRESULT Add_PartObjects() override;
	virtual HRESULT Add_Ability() override;
	void	Show_ImGUI();

private:
	STATE		m_eState = { STATE_END };

#ifdef DEBUG
	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };
#endif

public:
	static CChr_Battle_Sazh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END