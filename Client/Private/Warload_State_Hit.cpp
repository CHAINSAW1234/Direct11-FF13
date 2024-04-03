#include "stdafx.h"
#include "Warload_State_Hit.h"
#include "Warload.h"

CWarload_State_Hit::CWarload_State_Hit(CWarload* pWarload)
{
	m_pWarload = pWarload;
}

void CWarload_State_Hit::OnStateEnter()
{
}

void CWarload_State_Hit::OnStateUpdate(_float fTimeDelta)
{
	if (m_pWarload->Is_Animation_Finished())
		m_pWarload->Change_State(CWarload::STATE_IDLE);
}

void CWarload_State_Hit::OnStateExit()
{
}

CWarload_State_Hit* CWarload_State_Hit::Create(CWarload* pWarload)
{
	CWarload_State_Hit* pInstance = new CWarload_State_Hit(pWarload);

	return pInstance;
}

void CWarload_State_Hit::Free()
{
	__super::Free();
}
