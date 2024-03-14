#include "stdafx.h"
#include "Chr_Battle_Light_Move.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_Move::CChr_Battle_Light_Move(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}

void CChr_Battle_Light_Move::OnStateEnter()
{
}

void CChr_Battle_Light_Move::OnStateUpdate(_float fTimeDelta)
{
}

void CChr_Battle_Light_Move::OnStateExit()
{
}

void CChr_Battle_Light_Move::Update_Animation()
{
}

CChr_Battle_Light_Move* CChr_Battle_Light_Move::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Move* pInstance = new CChr_Battle_Light_Move(pChr_Battle_Light);

	return pInstance;
}

void CChr_Battle_Light_Move::Free()
{
	__super::Free();
}
