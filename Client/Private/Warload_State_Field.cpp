#include "stdafx.h"
#include "Warload_State_Field.h"
#include "Warload.h"

CWarload_State_Field::CWarload_State_Field(CWarload* pWarload)
{
	m_pWarload = pWarload;
}

void CWarload_State_Field::OnStateEnter()
{
	m_pWarload->Get_Transform()->Look_At_ForLandObject({ 65.f, 0.f,61.f,1.f });

	m_pWarload->Change_Animation(CWarload::FIELD_IDLE, true);
}

void CWarload_State_Field::OnStateUpdate(_float fTimeDelta)
{
}

void CWarload_State_Field::OnStateExit()
{
}

CWarload_State_Field* CWarload_State_Field::Create(CWarload* pWarload)
{
	CWarload_State_Field* pInstance = new CWarload_State_Field(pWarload);

	return pInstance;
}

void CWarload_State_Field::Free()
{
	__super::Free();
}
