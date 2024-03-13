#include "stdafx.h"
#include "Chr_Field_State_Move.h"

CChr_Field_State_Move::CChr_Field_State_Move(CChr_Field* pChr_Field)
{
}

void CChr_Field_State_Move::OnStateEnter()
{
}

void CChr_Field_State_Move::OnStateUpdate(_float fTimeDelta)
{
}

void CChr_Field_State_Move::OnStateExit()
{
}

CChr_Field_State_Move* CChr_Field_State_Move::Create(CChr_Field* pChr_Field)
{
	CChr_Field_State_Move* pInstance = new CChr_Field_State_Move(pChr_Field);
	//m_pPlayer = pGameObject;

	return pInstance;
}

void CChr_Field_State_Move::Free()
{
	__super::Free();
}
