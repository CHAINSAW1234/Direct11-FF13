#include "stdafx.h"
#include "Leopard_State_Attack.h"
#include "Leopard.h"

CLeopard_State_Attack::CLeopard_State_Attack(CLeopard* pLeopard)
{
	m_pLeopard = pLeopard;
}

void CLeopard_State_Attack::OnStateEnter()
{
	m_eState = RUN;
	m_pLeopard->Reset_Attakable();
	m_pLeopard->Change_Animation(CLeopard::RUN_START, false);
}

void CLeopard_State_Attack::OnStateUpdate(_float fTimeDelta)
{
	switch (m_eState) {
	case RUN:
		Run(fTimeDelta);
		break;
	case ATTACK:
		Attack(fTimeDelta);
		break;
	}
}

void CLeopard_State_Attack::OnStateExit()
{
	m_pLeopard->Reset_AttackTime();
	m_pLeopard->Change_Target();
}

void CLeopard_State_Attack::Run(_float fTimeDelta)
{
	_float fDegree = m_pLeopard->Cal_Degree_Target();
	_float fDist = m_pLeopard->Cal_Dist_Target();

	if (abs(fDegree) > 5) {
		m_pLeopard->Get_Transform()->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * fDegree / (fDegree));
	}
	else {
		m_pLeopard->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pLeopard->Get_TargetPosition()));
	}

	if (fDist <= .5f) {
		m_eState = ATTACK;
		m_pLeopard->Change_Animation(CLeopard::ATTACK, false);
	}

	m_pLeopard->Get_Transform()->Go_Straight(fTimeDelta * 3, m_pLeopard->Get_Navigation());

	if (m_pLeopard->Is_Animation_Finished())
		m_pLeopard->Change_Animation(CLeopard::RUN_IDLE, true);
}

void CLeopard_State_Attack::Attack(_float fTimeDelta)
{
	m_pLeopard->Check_Interact_Weapon();

	if (23.f <= m_pLeopard->Get_CurrentTrackPosition() &&
		m_pLeopard->Get_CurrentTrackPosition() <= 33.f)
		m_pLeopard->Get_Transform()->Go_Backward(fTimeDelta*3, m_pLeopard->Get_Navigation());

	if (m_pLeopard->Is_Animation_Finished())
		m_pLeopard->Change_State(CLeopard::STATE_IDLE);

}

CLeopard_State_Attack* CLeopard_State_Attack::Create(CLeopard* pLeopard)
{
	CLeopard_State_Attack* pInstance = new CLeopard_State_Attack(pLeopard);

	return pInstance;
}

void CLeopard_State_Attack::Free()
{
	__super::Free();
}
