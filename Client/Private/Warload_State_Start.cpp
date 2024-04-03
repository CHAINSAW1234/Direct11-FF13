#include "stdafx.h"
#include "Warload_State_Start.h"
#include "Warload.h"

CWarload_State_Start::CWarload_State_Start(CWarload* pWarload)
{
	m_pWarload = pWarload;
}

void CWarload_State_Start::OnStateEnter()
{
}

void CWarload_State_Start::OnStateUpdate(_float fTimeDelta)
{
}

void CWarload_State_Start::OnStateExit()
{
}

CWarload_State_Start* CWarload_State_Start::Create(CWarload* pWarload)
{
	CWarload_State_Start* pInstance = new CWarload_State_Start(pWarload);
	return pInstance;
}

void CWarload_State_Start::Free()
{
	__super::Free();
}
