#include "stdafx.h"
#include "Warload_State_Attack.h"
#include "Warload.h"

CWarload_State_Attack::CWarload_State_Attack(CWarload* pWarload)
{
	m_pWarload = pWarload;
}

void CWarload_State_Attack::OnStateEnter()
{
	m_eState = RUN;
	m_pWarload->Reset_Attakable();
	m_pWarload->Change_Animation(CWarload::RUN_START, false);
}

void CWarload_State_Attack::OnStateUpdate(_float fTimeDelta)
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

void CWarload_State_Attack::OnStateExit()
{
	m_pWarload->Set_IsJumpNext(true);
	m_pWarload->Reset_AttackTime();
}

void CWarload_State_Attack::Run(_float fTimeDelta)
{
	_float fDist = m_pWarload->Cal_Dist_Target();

	m_pWarload->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pWarload->Get_TargetPosition()));
	
	if (fDist <= 1) {
		m_eState = ATTACK;
		m_pWarload->Change_Animation(CWarload::ATTACK, false);
	}

	m_pWarload->Get_Transform()->Go_Straight(fTimeDelta * 30, m_pWarload->Get_Navigation());

	if (m_pWarload->Is_Animation_Finished())
		m_pWarload->Change_Animation(CWarload::RUN_IDLE, true);
}

void CWarload_State_Attack::Attack(_float fTimeDelta)
{
	m_pWarload->Check_Interact_Weapon();

	if (30.f <= m_pWarload->Get_CurrentTrackPosition() &&
		m_pWarload->Get_CurrentTrackPosition() <= 45.f)
		m_pWarload->Get_Transform()->Go_Backward(fTimeDelta*2, m_pWarload->Get_Navigation());

	if (m_pWarload->Is_Animation_Finished())
		m_pWarload->Change_State(CWarload::STATE_IDLE);
}

CWarload_State_Attack* CWarload_State_Attack::Create(CWarload* pWarload)
{
	CWarload_State_Attack* pInstance = new CWarload_State_Attack(pWarload);

	return pInstance;
}
void CWarload_State_Attack::Free()
{
	__super::Free();
}
