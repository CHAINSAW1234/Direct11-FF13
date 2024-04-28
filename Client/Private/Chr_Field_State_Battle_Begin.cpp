#include "stdafx.h"
#include "Chr_Field_State_Battle_Begin.h"
#include "Chr_Field.h"

CChr_Field_State_Battle_Begin::CChr_Field_State_Battle_Begin(CChr_Field* pChr_Field)
{
	m_pChr_Field = pChr_Field;
}

void CChr_Field_State_Battle_Begin::OnStateEnter()
{
	m_fTimeDelta = 0.f;
	m_pChr_Field->Change_Animation(CChr_Field::BATTLE_BEGIN_IDLE, false);
	m_pChr_Field->Change_Animation_Weapon(CChr_Field::WEAPON_OPEN);
	m_pGameInstance->PlaySoundW(TEXT("Light_Battle_Begin.wav"), CSound_Manager::HIT1, SOUND_DEFAULT);
}

void CChr_Field_State_Battle_Begin::OnStateUpdate(_float fTimeDelta)
{
	// 카메라 무브?
	
	m_fTimeDelta += fTimeDelta;

	if (m_fTimeDelta >= 1.5) {
		m_pChr_Field->Open_Level();
	}

	//if (m_pChr_Field->Is_Animation_Finished())
	//	m_pChr_Field->Change_State(CChr_Field::IDLE);

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
