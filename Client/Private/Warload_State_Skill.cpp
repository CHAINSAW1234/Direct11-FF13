#include "stdafx.h"
#include "Warload_State_Skill.h"
#include "Warload.h"
#include "UI_Number.h"
#include "UI_Skill.h"
#include "Effect.h"

CWarload_State_Skill::CWarload_State_Skill(CWarload* pWarload)
{
	m_pWarload = pWarload;
}

void CWarload_State_Skill::OnStateEnter()
{
	Change_State(CHARGE);
	m_iCount = 0;
	m_pWarload->Change_Animation(CWarload::SKILL_START, false);

	m_pUI_Skill = m_pWarload->Create_UI_Skill(TEXT("마도 디바이스 발동"));
	CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Hit_Air_Effect.dat", m_pGameInstance, m_pWarload->Get_Device(), m_pWarload->Get_Context(), m_pWarload->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION));
	m_pGameInstance->PlaySoundDuplicate(TEXT("Warload_Skill_Start.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);
}

void CWarload_State_Skill::OnStateUpdate(_float fTimeDelta)
{
	m_fTimeDelta += fTimeDelta;

	switch (m_eState)
	{
	case CHARGE:
		Charge(fTimeDelta);
		break;
	case HEAL:
		Heal(fTimeDelta);
		break;
	}

}

void CWarload_State_Skill::OnStateExit()
{
	if (nullptr != m_pUI_Skill)
		m_pUI_Skill->Set_Dead(true);
	m_pUI_Skill = nullptr;
}

void CWarload_State_Skill::Change_State(STATE eState)
{
	m_eState = eState;
	m_fTimeDelta = 0.f;
}

void CWarload_State_Skill::Charge(_float fTimeDelta)
{
	if (m_iCount < 2 && m_fTimeDelta >= 4.f/ (m_iCount*2+1)) {
		Change_State(HEAL);
		m_pWarload->Change_Animation(CWarload::SKILL_HEAL_START, false);
	}

	if (m_pWarload->Get_CurrentAnimationIndex() == CWarload::SKILL_IDLE && m_iCount >= 2) {
		m_pWarload->Change_Animation(CWarload::SKILL_END, false);
	}

	if (m_pWarload->Is_Animation_Finished()) {
		if (m_pWarload->Get_CurrentAnimationIndex() == CWarload::SKILL_END)
			m_pWarload->Change_State(CWarload::STATE_IDLE);
		else
			m_pWarload->Change_Animation(CWarload::SKILL_IDLE, true);
	}
}

void CWarload_State_Skill::Heal(_float fTimeDelta)
{
	if (m_pWarload->Is_Animation_Finished()) {
		if (m_pWarload->Get_CurrentAnimationIndex() == CWarload::SKILL_HEAL_START)
			m_pWarload->Change_Animation(CWarload::SKILL_HEAL_IDLE, false);
		else if (m_pWarload->Get_CurrentAnimationIndex() == CWarload::SKILL_HEAL_IDLE) {
			++m_iCount;
			m_pWarload->Add_Hp(310);
			m_pGameInstance->PlaySoundDuplicate(TEXT("Cure_Hit.wav"), CSound_Manager::EFFECT_DUPLICATE, SOUND_DEFAULT);

			_float4 vPos = m_pWarload->Get_Transform()->Get_State_Float4(CTransform::STATE_POSITION);
			vPos.y += m_pWarload->Get_ColliderSize().y * 0.5;
			if (FAILED(CEffect::Read_File_NoLoop("../Bin/Resources/Effect/Heal_Camera_Look_Instance.dat", m_pGameInstance, m_pWarload->Get_Device(), m_pWarload->Get_Context(), vPos)))
				return;

			m_pWarload->Change_Animation(CWarload::SKILL_HEAL_END, false);
			Change_State(CHARGE);
		}

	}
}

CWarload_State_Skill* CWarload_State_Skill::Create(CWarload* pWarload)
{
	CWarload_State_Skill* pInstance = new CWarload_State_Skill(pWarload);

	return pInstance;
}

void CWarload_State_Skill::Free()
{
	__super::Free();
}
