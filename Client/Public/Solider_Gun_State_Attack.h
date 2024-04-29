#pragma once
#include "FSM_State.h"

BEGIN(Client)
class CSolider_Gun_State_Attack final : public CFSM_State
{
private:
	CSolider_Gun_State_Attack(class CSolider_Gun* pSolider_Gun);
	virtual ~CSolider_Gun_State_Attack() = default;

public:
	// ���� �������� �� ó���� ȣ��
	virtual void OnStateEnter() override;
	// ���� ���ԵǾ� �ִ� ���¿��� �� tick���� ȣ��
	virtual void OnStateUpdate(_float fTimeDelta) override;
	// ���°� ����� �� ȣ��
	virtual void OnStateExit() override;

private:
	void Attack(_float fTimeDelta);

private:
	class CSolider_Gun* m_pSolider_Gun = { nullptr };
	_bool m_isAttack[3] = { false };

public:
	static	CSolider_Gun_State_Attack* Create(class CSolider_Gun* pSolider_Gun);
	virtual void Free() override;

};

END