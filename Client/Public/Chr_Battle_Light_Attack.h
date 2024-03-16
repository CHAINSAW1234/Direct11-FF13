#pragma once
#include "Client_Defines.h"
#include "FSM_State.h"

BEGIN(Client)
class CChr_Battle_Light_Attack final : public CFSM_State
{
	enum STATE { RUN, UP, DOWN, ATTACK, SKILL, FINISH, STATE_END };
	enum LAND { GROUND, AIR, LAND_END };
private:
	CChr_Battle_Light_Attack(class CChr_Battle_Light* pChr_Battle_Light);
	virtual ~CChr_Battle_Light_Attack() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Run(_float fTimeDelta);
	void Up(_float fTimeDelta);
	void Down(_float fTimeDelta);
	void Attack(_float fTimeDelta);
	void Skill(_float fTimeDelta);
	void Finish(_float fTimeDelta);

	void Update_LookAt();			// Target ��ġ�� �� Look���Ͱ��� ������ ����ؼ� ������

private:
	class CChr_Battle_Light* m_pChr_Battle_Light = { nullptr };
	STATE m_eState = { STATE_END };
	LAND m_eLand = { LAND_END };

	_float m_fDegree = { 0.f };
	_bool	m_testAttack = { false };

public:
	static	CChr_Battle_Light_Attack* Create(class CChr_Battle_Light* pChr_Battle_Light);
	virtual void Free() override;

};

END
