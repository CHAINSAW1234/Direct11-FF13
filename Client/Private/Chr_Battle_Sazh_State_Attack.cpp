#include "stdafx.h"
#include "Chr_Battle_Sazh_State_Attack.h"
#include "Chr_Battle_Sazh.h"
#include "UI_Skill.h"

CChr_Battle_Sazh_State_Attack::CChr_Battle_Sazh_State_Attack(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	m_pChr_Battle_Sazh = pChr_Battle_Sazh;
}

void CChr_Battle_Sazh_State_Attack::OnStateEnter()
{
	m_isCommandFinish = false;
	m_isAttack[0] = { false };
	m_isAttack[1] = { false };

	m_pChr_Battle_Sazh->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Sazh->Get_Target_Position()));
	m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::ATTACK_NOR1, false);

	CRole::SKILL eSkill = m_pChr_Battle_Sazh->Get_Current_Command();
	m_pUI_Skill = m_pChr_Battle_Sazh->Create_UI_Skill(eSkill);
	PlaySound();
}

void CChr_Battle_Sazh_State_Attack::OnStateUpdate(_float fTimeDelta)
{
	Attack();
}

void CChr_Battle_Sazh_State_Attack::OnStateExit()
{
	if (nullptr != m_pUI_Skill)
		m_pUI_Skill->Set_Dead(true);
	m_pUI_Skill = nullptr;
}

void CChr_Battle_Sazh_State_Attack::Attack()
{
	m_pChr_Battle_Sazh->Get_Transform()->Look_At_ForLandObject(XMLoadFloat4(&m_pChr_Battle_Sazh->Get_Target_Position()));

	if (!m_isCommandFinish) {

		if (!m_isAttack[0] && m_pChr_Battle_Sazh->Get_CurrentTrackPosition() >= 20) {
			m_isAttack[0] = true;
			m_pChr_Battle_Sazh->Create_Bullet(1);
		}

		if (!m_isAttack[1] && m_pChr_Battle_Sazh->Get_CurrentTrackPosition() >= 22) {
			m_isAttack[1] = true;
			m_pChr_Battle_Sazh->Create_Bullet(2);
		}

		if (m_pChr_Battle_Sazh->Get_CurrentTrackPosition() >= 25) {
			m_pChr_Battle_Sazh->Use_Command();
			CRole::SKILL eSkill = m_pChr_Battle_Sazh->Get_Current_Command();

			if (eSkill == CRole::FLAMEBLOW) {
				m_pChr_Battle_Sazh->Change_Animation(CChr_Battle_Sazh::ANIMATION_CHR_BATTLE_SAZH(m_pChr_Battle_Sazh->Get_CurrentAnimationIndex() + 1), false);
				m_isAttack[0] = false;
				PlaySound();
				m_isAttack[1] = false;
			}
			else {
				m_isCommandFinish = true;
			}
		}	
	}
	else {
		if (m_pChr_Battle_Sazh->Is_Animation_Finished()) {
			m_pChr_Battle_Sazh->Change_State(CChr_Battle_Sazh::IDLE);
		}
	}
}

void CChr_Battle_Sazh_State_Attack::PlaySound()
{
	_int iRand = rand() % 3;
	if (!iRand) {
		_int irand = rand() % 5;
		wstring strSoundTag = L"Sazh_Shout_" + to_wstring(irand) + L".wav";
		const TCHAR* pChar = strSoundTag.c_str();
		m_pGameInstance->PlaySoundW(const_cast<TCHAR*>(strSoundTag.c_str()), CSound_Manager::CHR2, SOUND_DEFAULT);
	}
}

CChr_Battle_Sazh_State_Attack* CChr_Battle_Sazh_State_Attack::Create(CChr_Battle_Sazh* pChr_Battle_Sazh)
{
	CChr_Battle_Sazh_State_Attack* pInstance = new CChr_Battle_Sazh_State_Attack(pChr_Battle_Sazh);

	return pInstance;
}

void CChr_Battle_Sazh_State_Attack::Free()
{
	__super::Free();
}
