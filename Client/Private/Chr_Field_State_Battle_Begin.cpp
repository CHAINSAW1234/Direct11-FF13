#include "stdafx.h"
#include "Chr_Field_State_Battle_Begin.h"
#include "Chr_Field.h"

CChr_Field_State_Battle_Begin::CChr_Field_State_Battle_Begin(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Battle_Begin::OnStateEnter()
{

}

void CChr_Field_State_Battle_Begin::OnStateUpdate(_float fTimeDelta)
{
	// 카메라 무브?
	
	if (m_pChr_Field->Is_Animation_Finished())
		m_pChr_Field->Change_State(CChr_Field::IDLE);

}

void CChr_Field_State_Battle_Begin::OnStateExit()
{
}

CChr_Field_State_Battle_Begin* CChr_Field_State_Battle_Begin::Create(CChr_Field* pChr_Field)
{
	CChr_Field_State_Battle_Begin* pInstance = new CChr_Field_State_Battle_Begin(pChr_Field);
	//m_pPlayer = pGameObject;

	return pInstance;
}

void CChr_Field_State_Battle_Begin::Free()
{
	__super::Free();
}
