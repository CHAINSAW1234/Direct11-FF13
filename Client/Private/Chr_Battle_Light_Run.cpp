#include "stdafx.h"
#include "Chr_Battle_Light_Run.h"
#include "Chr_Battle_Light.h"

CChr_Battle_Light_Run::CChr_Battle_Light_Run(CChr_Battle_Light* pChr_Battle_Light)
{
	m_pChr_Battle_Light = pChr_Battle_Light;
}
void CChr_Battle_Light_Run::OnStateEnter()
{
}
void CChr_Battle_Light_Run::OnStateUpdate(_float fTimeDelta)
{
}
void CChr_Battle_Light_Run::OnStateExit()
{
}
void CChr_Battle_Light_Run::Update_Animation()
{
}
CChr_Battle_Light_Run* CChr_Battle_Light_Run::Create(CChr_Battle_Light* pChr_Battle_Light)
{
	CChr_Battle_Light_Run* pInstance = new CChr_Battle_Light_Run(pChr_Battle_Light);

	return pInstance;
}
void CChr_Battle_Light_Run::Free()
{
	__super::Free();
}