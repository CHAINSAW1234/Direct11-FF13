#pragma once
#include "Client_Defines.h"
#include "Chr_Battle.h"

BEGIN(Client)
class CChr_Battle_Vanila final : public CChr_Battle
{
public:
	enum STATE { IDLE, ATTACK, SKILL, HIT, DEAD, FINISH, STATE_END };
	enum ANIMATION_CHR_BATTLE_VANILA {	// FINISH -> ÀüÅõ Á¾·á	// IDLE_IDLEÀº STATE¶û °ãÃ³¼­ ¸¸µë
		DEAD_END, DEAD_IDLE, DEAD_START, 
		HIT_FALL,  HIT_FALL_WAKE, HIT_FALL_DEAD,
		ANIM_FINISH, HIT_BACK, HIT_LEFT, HIT_RIGHT, HIT_STRAIGTH,
		HURT_END, HURT_IDLE, HURT_START, ANIM_IDLE,
		MOVE_BACK_IDLE, MOVE_BACK_STOP_LEFT, MOVE_BACK_STOP_RIGHT,
		MOVE_LEFT_IDLE, MOVE_LEFT_STOP_LEFT, MOVE_LEFT_STOP_RIGHT, MOVE_RIGHT_END, MOVE_RIGHT_IDLE, MOVE_RIGHT_START,
		MOVE_RIGHT_IDLE, MOVE_RIGHT_STOP_LEFT, MOVE_RIGHT_STOP_RIGHT, MOVE_RIGHT_END, MOVE_RIGHT_IDLE, MOVE_RIGHT_START,
		MOVE_STRAIGHT_IDLE, MOVE_STRAIGHT_START, MOVE_STRAIGHT_STOP_LEFT, MOVE_STRAIGHT_STOP_RIGHT,
		OPTIMACHANGE, SKILL, TURN_LEFT, TURN_RIGHT, ANIMATION_END
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
	HRESULT Change_State(STATE eState);
	void	Change_Animation(ANIMATION_CHR_BATTLE_VANILA iAnimationIndex, _bool isLoop);

private:
	virtual HRESULT Add_Components() override;
	virtual HRESULT Add_Component_FSM() override;
	virtual HRESULT Add_PartObjects() override;
	virtual HRESULT Add_Ability() override;

	void	Show_ImGUI();

private:
	STATE		m_eState = { STATE_END };
	vector<CChr_Battle*>	m_Memebers;						// Èú ÇÏ±â À§ÇÔ 
	class CImGUI_Manager* m_pImGUI_Manager = { nullptr };

public:
	static CChr_Battle_Vanila* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;
};

END