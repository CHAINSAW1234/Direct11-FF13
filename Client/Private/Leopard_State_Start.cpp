#include "stdafx.h"
#include "Leopard_State_Start.h"
#include "Leopard.h"

CLeopard_State_Start::CLeopard_State_Start(CLeopard* pLeopard)
{
	m_pLeopard = pLeopard;
}

void CLeopard_State_Start::OnStateEnter()
{
	m_pLeopard->Change_Animation(CLeopard::BATTLE_START, false);
}

void CLeopard_State_Start::OnStateUpdate(_float fTimeDelta)
{
}

void CLeopard_State_Start::OnStateExit()
{
}

CLeopard_State_Start* CLeopard_State_Start::Create(CLeopard* pLeopard)
{
	CLeopard_State_Start* pInstance = new CLeopard_State_Start(pLeopard);

	return pInstance;
}

void CLeopard_State_Start::Free()
{
	__super::Free();
}
